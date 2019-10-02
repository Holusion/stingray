#include "../config.h"
#include  "debug.h"
#include  "display.hpp"
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




void run(int argc, char ** args){
  core::Display display; //SDL init / teardown
  EventManager   manager;
  std::shared_ptr<Video> video;
  #ifdef ENABLE_DBUS
    //FIXME if it already exists on bus, call an Open action instead of starting a new window
    dbus::DBus    bus(&manager);
  #endif
  DEBUG_LOG("Initialization complete"<<std::endl);
  if(argc == 2) {
    DEBUG_LOG("Openning : "<<args[1]<<std::endl);
    video = std::make_shared<Video>(args[1],display.getWidth(),display.getHeight(), 0);
    display.setSource(video);
    video.reset();
  }
  state_t s = manager.update();

  //Main thread event loop
  while(!s.quit){
    s = manager.update();
    std::string next_video = bus.pop_play_next();
    video = display.getSource();
    if(next_video != "" &&(! video || video->filename != next_video)){
      DEBUG_LOG("Next video : "<<next_video<<std::endl);
      #ifdef ENABLE_SEAMLESS
        std::shared_ptr<entities::Video> newVideo = std::make_shared<Video>(next_video, display.getWidth(), display.getHeight(), ((video)?video->buffer.index() : 0));
      #else
        std::shared_ptr<entities::Video> newVideo = std::make_shared<Video>(next_video, display.getWidth(), display.getHeight(), 0);
      #endif
      display.setSource(newVideo);
    } //*/
    if(video){
      video->pause = (s.axis == 0) ? true : false;
      video->speed = (video->context.fps *std::abs(s.axis))/4;
      if ( (s.axis < 0  && video->buffer.direction() == Direction::NORMAL)
          || (0 < s.axis  && video->buffer.direction() == Direction::REVERSE)) {
        video->buffer.swap();
      }
    }
    
    display.draw();
    //*/
    /*
    if(video != NULL) {
      if(manager.nextVideo.compare("") != 0) {
        manager.changeVideoState();
      }
      manager.update(*video);
    }else{
      manager.update();
    }
    //*/
  }
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
