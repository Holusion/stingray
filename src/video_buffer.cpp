#include "video_buffer.hpp"

using namespace entities;

void  VideoBuffer::pop_to_back() {

  Base::m_backData.push_front(std::move(Base::m_data.front()));
  if (Base::m_backData.size() > Base::m_maxSize) {
    delete Base::m_backData.back();
    Base::m_backData.pop_back();
  }
  Base::m_data.pop_front();
}
void VideoBuffer::switchDirection() {

  //! Lock Mutex RAII style
  std::lock_guard<std::mutex> lock(m_lock);

  Base::swap();
  m_direction = (m_direction == E_Direction::NORMAL) ? E_Direction::REVERSE : E_Direction::NORMAL;
  m_fullClean = true;

}

int  VideoBuffer::write(std::array<VideoFrame*, DECODE_SIZE>& data, E_Direction direction) {

    int  add = DECODE_SIZE;

    //! Lock Mutex RAII style
    std::lock_guard<std::mutex> lock(m_lock);

    std::deque<VideoFrame*>&  queue = (direction == m_direction) ? Base::m_data : Base::m_backData;

    //! Check amount of data that we can add
    if (queue.size() + data.size() > Base::m_maxSize)
      add = Base::m_maxSize - queue.size();

    //! We cann add somthing
    if (add > 0) {

    //! Insert data
    queue.insert(queue.end(),
        data.begin(),
        data.begin() + add);

    }
    else
      add = 0;

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

  //! Lock Mutex RAII style
  std::lock_guard<std::mutex> lock(m_lock);
  // We need to lock for read since we do'nt have a lock dedicated to direction change

 if (Base::m_data.size() <= 0)
    return nullptr;

  pop_to_back();

  //! Update Index
  if (m_direction == E_Direction::REVERSE
      && m_index == 0)
    m_index = m_lastIndex;
  else if (m_direction == E_Direction::NORMAL
      && m_index >= m_lastIndex)
    m_index = 0;
  else
    (m_direction == E_Direction::NORMAL) ? ++m_index : --m_index;

  VideoFrame* ret = Base::m_backData.front();


  return ret;
}
