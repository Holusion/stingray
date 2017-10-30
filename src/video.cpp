#include "video.hpp"

using namespace entities;

Video::Video(const char* file,int width,int height) : context(file, width, height),
                                 pause(false),
                                 speed(this->context.fps),
                                 buffer(new DeBuffer<VideoFrame*>(context.nbFrames)),
                                 alpha(0) {
}

Video::~Video(){
  delete buffer;
}
