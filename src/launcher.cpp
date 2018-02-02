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
#ifdef ENABLE_DBUS
#include  "dbus/dbus.hpp"
#endif
#include  <thread>
#include  <chrono>

using namespace  std::chrono;
using namespace  entities;
#ifdef ENABLE_DBUS
using namespace  dbus;
#endif

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

    try {
      while(!manager->isEnd() && manager->currentState != switch_state && manager->currentState != not_play){
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

#ifdef ENABLE_DBUS
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
#endif

void run(int argc, char ** args){
  core::Window  window;
  EventManager   manager;
  #ifdef ENABLE_DBUS
  dbus::DBus    bus(&manager);
  DBusThread            *listener = new DBusThread(&bus, &manager);
  #endif
  entities::Video* video = NULL;
  DecodeThread          *decoder = NULL;

  if(argc == 2) {
    video = new entities::Video(args[1],window.getWidth(),window.getHeight());
    decoder = new DecodeThread(video,&manager);
    manager.currentState = fade_in;
  }

  while(!manager.isEnd()){
    if(video == NULL && manager.currentState == fade_out) {
      manager.currentState = switch_state;
    }

    if(video != NULL) {
      manager.update(*video);
      window.draw(*video);
    }

    if(manager.currentState == switch_state && manager.nextVideo.compare("") != 0) {
      delete decoder;
      delete video;
      video = new entities::Video(manager.nextVideo.c_str(), window.getWidth(), window.getHeight());
      manager.nextVideo.erase();
      manager.currentState = fade_in;
      decoder = new DecodeThread(video,&manager);
    }
  }
  delete decoder;
  #ifdef ENABLE_DBUS
  delete listener;
  #endif
  delete video;
}

int  main (int argc, char** argv) {
  DEBUG_LOG("Debug Mode Enabled"<<std::endl);
  try {
    run(argc, argv);
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
