#include "video_buffer.hpp"

using namespace entities;

void VideoBuffer::switchDirection() {
  buffer.swap();
}

int  VideoBuffer::write(std::array<VideoFrame*, DECODE_SIZE>& data, Direction direction) {

    int  add = DECODE_SIZE;

    //! Check amount of data that we can add
    if (buffer.size() + data.size() > buffer.limit()){
      add = buffer.limit() - buffer.size();
      std::cout<<"out of bounds array write attempt"<<std::endl;
    }

    //! We can add somthing
    if (add > 0) {
      // Insert data
      for(int i=0; i< add; i++){
        buffer.write(data[i],direction);
      }
    } else {
      add = 0; //Can it possibly be < 0 ?
    }

    //! Clean data classic
    for (int i = 0; i < add; ++i)
      data[i] = nullptr;

    if (m_fullClean) {
      //! Switch detected, clean all
      for (int i = add ; i < DECODE_SIZE; ++i) {
        delete data[i];
        data[i] = nullptr;
      }
      add = DECODE_SIZE;
      m_fullClean = false;
    }

    return add;
}

VideoFrame*  VideoBuffer::read() {
  return buffer.forward();
}
