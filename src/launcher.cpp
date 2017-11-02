#include "../config.h"
#include  "constants.h"
#include  "debug.h"
#include  "window.hpp"
#include  "video_decoder.hpp"
#include  "video_frame.hpp"
#include  "events/event_manager.hpp"
#include  "exceptions/global_exception.hpp"
#include  "exceptions/sdl_exception.hpp"
#include  "exceptions/av_exception.hpp"
#include  "dbus/dbus.hpp"
#include  <thread>
#include  <chrono>

using namespace  std::chrono;
using namespace  entities;
using namespace  dbus;

class DecodeThread{
public:
  std::thread th;
  DecodeThread(entities::Video* v,EventManager* m):
    th(DecodeThread::decode_loop, v, m) { }
  ~DecodeThread(){
    //thread is auto-stopped by manager
    //We just need to join().
    th.join();
  }
  static void decode_loop (entities::Video* video,EventManager* manager){
    bool blocked = false;
    decoder::VideoDecoder decoder(video->context);

    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    try {
      while(!manager->isEnd() && strcmp(manager->currentState, "switch")){
        //std::this_thread::sleep_for(std::chrono::milliseconds(100)); //FIXME try something else, the old sleep was bad (100% cpu here)
        if(video->buffer->size() + DECODE_SIZE < video->buffer->limit()){
          blocked = false;
          decoder.decodeAndWrite(*video->buffer);
        }else{ //Don't even decode if the buffer is nearly full.
          if (!blocked){
            DEBUG_LOG("Blocking : Buffer is full"<<std::endl);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
          }else{
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
          }
          blocked = true;
        }
      }
    }catch (AVException& e){
      std::cerr <<"Decode Thread -> AV Exception : "<< e.what() <<std::endl;
    }catch (GlobalException& e) {
      std::cerr << "Decode Thread -> " << e.what() << std::endl;
    }
  }
};

class DBusThread {
public:
  std::thread th;

  DBusThread(dbus::DBus* bus, EventManager* manager):
    th(DBusThread::listen_loop, bus, manager) {}

  ~DBusThread() {
    th.join();
  }

  static void listen_loop(dbus::DBus* bus, EventManager* manager) {
    while(!manager->isEnd()) {
      bus->update();
    }
  }
};

void run(char ** args){
  core::Window  window;
  EventManager   manager;
  dbus::DBus    bus(&manager);
  entities::Video* video = new entities::Video(args[1],window.getWidth(),window.getHeight());
  DecodeThread          *decoder = new DecodeThread(video,&manager);
  DBusThread            *listener = new DBusThread(&bus, &manager);

  while(!manager.isEnd()){
    manager.update(*video);
    window.draw(*video);

    if(strcmp(manager.currentState, "switch") == 0) {
      delete decoder;
      video = new entities::Video(manager.nextVideo, window.getWidth(), window.getHeight());
      manager.nextVideo = "";
      manager.currentState = "fadeIn";
      decoder = new DecodeThread(video,&manager);
    }
  }
  delete decoder;
  delete listener;
}

int  main (int argc, char** argv) {
  if (argc != 2){
    return EXIT_FAILURE;
  }
  DEBUG_LOG("Debug Mode Enabled"<<std::endl);
  try {
    run(argv);
  }catch (SDLException& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }catch (AVException& e){
    std::cerr <<"AV Exception : "<< e.what() <<std::endl;
    return EXIT_FAILURE;
  }catch (GlobalException& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
