#include "video.hpp"

using namespace entities;

Video::Video(const char* file,int width,int height) : context(file, width, height),
                                 pause(false),
                                 speed(this->context.fps),
                                 buffer(new DeBuffer<VideoFrame*>(context.nbFrames)),
                                 alpha(0) {
}

Video::~Video(){
  for(int i = 1; i < buffer->sizeBackData(); i++) {
    if(buffer->getBackData(i) != nullptr) delete buffer->getBackData(i);
  }

  for(int i = 1; i < buffer->size(); i++) {
    if(buffer->get(i) != nullptr) delete buffer->get(i);
  }

  buffer->clean();
  delete buffer;
}
