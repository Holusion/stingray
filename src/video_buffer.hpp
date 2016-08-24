#ifndef  VIDEO_BUFFER_HPP
#define  VIDEO_BUFFER_HPP

#include  "dbuffer.hpp"
#include  "video_frame.hpp"
#include  <deque>
#include  <algorithm>
#include  <mutex>
#include  "exceptions/global_exception.hpp"
#include "constants.h"

enum class E_Direction {NORMAL, REVERSE};

//! @namespace entities
//! @brief Data namespace
namespace  entities {

  //! @class VideoBuffer
  //! @brief Class which contains all VideoFrame (Double Buffer pattern)
  class  VideoBuffer : public memory::DBuffer<std::deque, VideoFrame*> {

    typedef memory::DBuffer<std::deque, VideoFrame*>  Base;

    private:
      std::mutex                 m_lock;
      E_Direction                m_direction;
      unsigned int               m_index;
      const unsigned int         m_lastIndex;
      bool                       m_fullClean;

    public:
      VideoBuffer(unsigned int videoSize,
                  E_Direction direction) :
                                          Base((videoSize < CAPACITY) ? videoSize : CAPACITY),
                                          m_lock(),
                                          m_direction(direction),
                                          m_index(0),
                                          m_lastIndex(videoSize - 1){};
      VideoBuffer(unsigned int videoSize) : VideoBuffer(videoSize, E_Direction::NORMAL){};
      ~VideoBuffer() = default;

    private:
      void  pop_to_back();

    public:
      std::size_t   size() const { return Base::m_data.size(); }
      std::size_t   limit() const { return Base::m_maxSize; }
      unsigned int  index() const { return m_index; }
      E_Direction   direction() const {return m_direction; }
      void          switchDirection();
      int           write(std::array<VideoFrame*, DECODE_SIZE>& data, E_Direction direction);
      VideoFrame*   read();
  };
}
#endif
