#ifndef  VIDEO_BUFFER_HPP
#define  VIDEO_BUFFER_HPP

#include  "debuffer.hpp"
#include  "video_frame.hpp"
#include  <algorithm>
#include  "exceptions/global_exception.hpp"
#include "constants.h"

typedef Direction E_Direction;

//! @namespace entities
//! @brief Data namespace
namespace  entities {

  //! @class VideoBuffer
  //! @brief Class which contains all VideoFrame (Double Buffer pattern)
  //Transitionning to full use of DeBuffer
  class  VideoBuffer{

    private:
      DeBuffer<VideoFrame*>      buffer;
      bool                       m_fullClean;

    public:
      VideoBuffer(unsigned int videoSize,
                  Direction direction) :
                  buffer(videoSize, direction) {};

      VideoBuffer(unsigned int videoSize) : VideoBuffer(videoSize, Direction::NORMAL){};
      ~VideoBuffer() = default;

    public:
      std::size_t   size() const { return buffer.size(); }
      std::size_t   limit() const { return buffer.limit(); }
      unsigned int  index() const { return buffer.index(); }
      Direction   direction() const {return buffer.direction(); }
      void          switchDirection();
      int           write(std::array<VideoFrame*, DECODE_SIZE>& data, Direction direction);
      VideoFrame*   read();
  };
}
#endif
