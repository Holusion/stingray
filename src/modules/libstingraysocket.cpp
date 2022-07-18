#include "events/eventListener.hpp"
#include "debug.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include  <errno.h>
#include  <unistd.h>
#include  <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <csignal>
#include <vector>
#include <algorithm> // remove and remove_if
#include <regex>

#include <mutex>
#include <atomic>
const char resp[] = "HTTP/1.1 204 No Content\r\n\r\n";

class  Controller: public EventListener {
  public:
    Controller(int);
    ~Controller();
    void update();
    static void serve(int signum);
    static bool handle(int);
    static bool configure(int f);
    static std::string get_request(int);
    static void parse_request(const char*);
    static void parse_line(std::string);
    virtual int getAxis() const{return axis;};
    virtual int getQuit() const{return quit;};
  private:
    std::string sock_addr;
    static std::vector<int>clients;
    static std::atomic<int> fd;
    static std::atomic<int> angle;
    static std::atomic<int> axis;
    static std::atomic<int> quit;
    static std::mutex m;
    static std::regex re;
    static const size_t buflen_ = 1024;
    static Controller *instance;
};


std::atomic<int> Controller::fd(-1);
std::atomic<int> Controller::angle(0);
std::atomic<int> Controller::axis(4);
std::atomic<int> Controller::quit(0);

std::mutex Controller::m;
std::regex Controller::re = std::regex("GET /([md][+-]?[0-9.]+|QUIT)",std::regex::ECMAScript);
std::vector<int>Controller::clients = std::vector<int>();

Controller::Controller(int port=3004){
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(addr.sin_zero), 8);
  int err, flags, one=1;
  if (fd ==-1){
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0){
      std::cout<<"Failed to open Socket : "<<strerror(-fd)<<std::endl;
      return;
    }
    // set socket options

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    err = bind((int)fd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0){
      std::cout<<"Error binding "<<port<<": "<<strerror(-err)<<std::endl;
    }
    //set socket flags
    configure(fd);

    err =listen(fd,5);
    if (err == -1){
      std::cout<<"Error listening to  "<<port<<": "<<strerror(errno)<<std::endl;
    }
  }
  std::cout<<"Listening on port "<<port<<std::endl;
}

Controller::~Controller() {
  quit = 1;
  for(auto client: clients){
    close(client);
  }
  if(0 < fd){
    close(fd);
    fd = -1;
  }
  //t.join();
}

bool Controller::configure(int f){
  int flags;
  flags = fcntl (f, F_GETFL);
  if(fcntl (f, F_SETFL, flags|O_ASYNC|O_NONBLOCK)<0){
    std::cout<<"F_SETFL failed"<<std::endl;
    return false;
  }
  if(fcntl(f, F_SETSIG, SIGIO)<0){
    std::cout<<"F_SETSIG failed"<<std::endl;
    return false;
  }
  struct sigaction handler;
  handler.sa_handler = Controller::serve;
  handler.sa_flags = 0;
  if (sigfillset(&handler.sa_mask)< 0){
    std::cout<<"failed to set fillset"<<std::endl;
    return false;
  }
  if(sigaction(SIGIO,&handler,0) < 0){
    std::cout<<"sigaction() failed"<<std::endl;
    return false;
  }
  if(fcntl(f, F_SETOWN, getpid())<0){
    std::cout<<"F_SETOWN failed"<<std::endl;
    return false;
  }
  return true;
}

void Controller::serve(int signum){
  std::lock_guard<std::mutex> lock(m);
  // setup client
  int client, flags;
  struct sockaddr client_addr;
  socklen_t clientlen = sizeof(client_addr);
  if ((client = accept(fd, &client_addr,&clientlen)) < 0) {
    if (errno != EWOULDBLOCK && errno != EAGAIN){
      std::cout<<"Accept error : "<<strerror(errno)<<std::endl;
    }
  }else{
    DEBUG_LOG("Got a client!!"<<std::endl);
    //Configure the new client
    if(configure(client)){
      clients.push_back(client); //Append to clients
    }
  }
  //Then we handle accepted clients.
  //If handle() returns true, client is closed and should be deleted
  clients.erase(
    std::remove_if(std::begin(clients), std::end(clients), handle),
    std::end(clients)
  );
}

//return true if socket is closed
bool Controller::handle(int client){
  ssize_t rc;
  char buf [1024];
  rc = read(client, buf, 1024);
  if(rc == 0){
    rc = close(client);
    if(rc != 0){
      std::cout << "Error closing client : "<<strerror(errno)<<std::endl;
    }else{
      DEBUG_LOG("Closed client "<<client<<std::endl);
    }
    return true;
  }else if (rc == -1){
    if (errno == EWOULDBLOCK || errno == EAGAIN){
      return false;
    }else{
      std::cout<<"read error on "<<client<<" : "<<strerror(errno)<<std::endl;
      return true;
    }
  }else{
    parse_request(buf);
    rc = send(client, resp, strlen(resp), 0);
    if(rc == -1){
      DEBUG_LOG("Error sending response : "<<strerror(errno)<<std::endl);
    }
  }
  return false;
}

void Controller::parse_request(const char* buf){
  try{

    std::string str(buf);
    parse_line(str.substr(0, str.find("\n")));
  }catch (const std::invalid_argument& ia) {
	  std::cerr << "Invalid argument: " << ia.what()<<": "<<buf << '\n';
  }
}

void Controller::parse_line(std::string str){
  int na;
  std::smatch m;
  if (str == "QUIT"){
      quit = 1;
      return;
  }
  if (str[0] == 'd'){
    axis = 0;
    na = std::stoi(str.substr(1));
    if (std::signbit(na) == std::signbit((int)angle)){
      angle += na;
    }else{
      angle = na;
    }
  }else if(str[0] == 'm'){
    angle = 0;
    axis = std::stoi(str.substr(1));
  }else if(std::regex_search(str, m, re)){
    return parse_line(m[1]);
  }else{
    std::cerr << "Invalid command: "<<str << std::endl;
  }

}

void Controller::update(){
  //No need for mutex because all values are atomic
  if (angle == 0){
    //We work in angle mode only when axis = 0.
    return;
  }
  if( 4 < abs(angle)){
    axis = sqrt(abs(angle)*4)*abs(angle)/angle;
  }else{
    axis = 4*angle/abs(angle);
  }
  if(angle < 0) angle++;
  else if(0 < angle) angle--;
}



extern "C"{
  // the class factories
  extern EventListener* create() {
   return new Controller;
  }
  extern void destroy(EventListener* p) {
   delete p;
  }
  extern void update(EventListener* p){
    ((Controller*)p)->update();
  }
}
