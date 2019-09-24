#include "video.hpp"

using namespace entities;

Video::Video(const std::string file, int width, int height, std::size_t start_frame_number): 
  filename(file),
  context(file.c_str(), width, height),
  pause(false),
  speed(this->context.fps),
  //FIXME does inherit parent's position but not its direction...
  buffer(context.nbFrames, Direction::NORMAL, start_frame_number),
  #ifdef ENABLE_CROSSFADE
  alpha(0),
  #else
  alpha(UINT8_MAX),
  #endif
  state(in),
  size(context.nbFrames),
  th(Video::decode_loop, this) 
  {
    DEBUG_LOG("Opening new Video from " << filename <<std::endl);
  }

Video::~Video(){
  this->state = closed; //Cause thread to terminate among other things
  //Provide memory leaks, the buffer is not completely clean because it crashed when decoder try to decode empty buffer
  th.join();
  DEBUG_LOG("Destroyed Video "<< filename <<std::endl);
  /*
    for(int i = 1; i < buffer.sizeBackData(); i++) {
    if(buffer.getBackData(i) != nullptr) delete buffer.getBackData(i);
  }
  for(int i = 1; i < buffer.size(); i++) {
    if(buffer.get(i) != nullptr) delete buffer.get(i);
  }
  buffer.clean();//*/
}

void Video::decode_loop (entities::Video* video){
  bool blocked = false;
  decoder::VideoDecoder decoder(video->context);
  try {
    while(video->state != out && video->state != closed ){
      if(video->buffer.size() + DECODE_SIZE < video->buffer.limit()){
        blocked = false;
        decoder.decodeAndWrite(video->buffer);
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
    std::cerr <<"Decode Thread -> AV Exception "<< e.what() <<std::endl;
  }catch (GlobalException& e) {
    std::cerr << "Decode Thread -> " << e.what() << std::endl;
  }
}