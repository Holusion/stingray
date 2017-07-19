#include "video.hpp"

using namespace entities;

Video::Video(const char* file,int width,int height) : context(file, width, height),
                                 pause(false),
                                 speed(this->context.fps),
                                 buffer(new DeBuffer<VideoFrame*>(context.nbFrames)) {
}

Video::~Video(){
  delete buffer;
}
