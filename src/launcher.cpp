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
#include  <thread>
#include  <chrono>
using namespace  std::chrono;
using namespace  entities;

void decode_loop(entities::Video* video,EventManager* manager){
  bool blocked = false;
  decoder::VideoDecoder decoder(video->context);

  try {
    while(!manager->isEnd()){
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
void run(char ** args){
  core::Window  window;
  EventManager   manager;
  entities::Video        video(args[1],window.getWidth(),window.getHeight());
  std::thread            decode_thread(decode_loop,&video,&manager);

  while(!manager.isEnd()){
    manager.update(video);
    window.draw(video);
  }
  decode_thread.join();
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
