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
      unsigned int               m_index;
      const unsigned int         m_lastIndex;
      bool                       m_fullClean;

    public:
      VideoBuffer(unsigned int videoSize,
                  E_Direction direction) :
                                          buffer((videoSize < CAPACITY) ? videoSize : CAPACITY, direction),
                                          m_index(0),
                                          m_lastIndex(videoSize - 1){};
      VideoBuffer(unsigned int videoSize) : VideoBuffer(videoSize, E_Direction::NORMAL){};
      ~VideoBuffer() = default;

    public:
      std::size_t   size() const { return buffer.size(); }
      std::size_t   limit() const { return buffer.limit(); }
      unsigned int  index() const { return m_index; }
      E_Direction   direction() const {return buffer.direction(); }
      void          switchDirection();
      int           write(std::array<VideoFrame*, DECODE_SIZE>& data, E_Direction direction);
      VideoFrame*   read();
  };
}
#endif
