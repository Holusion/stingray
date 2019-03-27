#include "video.hpp"

using namespace entities;

Video::Video(const char* file, int width, int height, std::size_t start_frame_number): 
  context(file, width, height),
  pause(false),
  speed(this->context.fps),
  //FIXME does inherit parent's position but not its direction...
  buffer(new DeBuffer<VideoFrame*>(context.nbFrames, Direction::NORMAL, start_frame_number)),
  alpha(0),
  state(not_play),
  size(context.nbFrames) 
  {}

Video::~Video(){
  //Provide memory leaks, the buffer is not completely clean because it crashed when decoder try to decode empty buffer
  for(int i = 1; i < buffer->sizeBackData(); i++) {
    if(buffer->getBackData(i) != nullptr) delete buffer->getBackData(i);
  }

  for(int i = 1; i < buffer->size(); i++) {
    if(buffer->get(i) != nullptr) delete buffer->get(i);
  }

  buffer->clean();
  delete buffer;
}
