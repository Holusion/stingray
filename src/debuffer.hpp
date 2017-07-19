/* Copyright (C) 2016  Sebastien DUMETZ */
#ifndef  DEBUFFER_HPP
#define  DEBUFFER_HPP

#include  <deque>
#include  <mutex>
using namespace std;
//! @namespace  memory
//! @brief Memory namespace
enum class Direction {NORMAL, REVERSE};


//! @class DEBuffer
//! @brief Double buffer with templated read and write
template <class T> class  DeBuffer {
  //a deque ("deck") works like a vector but better for scattered data like our frames.
  typedef deque<T> buffer_t;

  protected:

    std::mutex         m;
    buffer_t           data; //Is swapped when direction changes
    buffer_t           backData; // always contain data of !direction.
    Direction          d;
    const std::size_t  maxSize;
  public:
    DeBuffer<T>(const std::size_t s, Direction d=Direction::NORMAL) :
                                    m(),
                                    maxSize(s),
                                    data(),
                                    d(d),
                                    backData() {}
    virtual ~DeBuffer<T>(void){};

  public:
    std::size_t size() const{return data.size();}
    std::size_t limit() const { return maxSize; }
    Direction   direction() const {return d; }
    void  swap();
    bool write(T item, Direction d);
    bool write(T item);
    T forward();
};

//Write with a direction, to allow for write-after-swap
template <class T>
bool DeBuffer<T>::write(T item, Direction write_d){
  // Lock Mutex RAII style
  std::lock_guard<std::mutex> lock(m);
  std::deque<T>& queue = (write_d == d)? data: backData;
  if (maxSize <= queue.size()){
    return false;
  }
  queue.push_back(item);
  return true;
}
/*
 * Templated code should ALWAYS be in a header file
 */
//Write with default direction
template <class T>
bool DeBuffer<T>::write(T item){
  return write(item, direction);
}

template <class T>
T DeBuffer<T>::forward(void){
  std::lock_guard<std::mutex> lock(m);
  if (data.size() == 0){
    return nullptr;
  }
  backData.push_front(std::move(data.front()));
  data.pop_front(); //Delete first after it has been moved
  if (maxSize < backData.size()) {
    delete backData.back();
    backData.pop_back();
  }
  return backData.front();
}

template <class T>
void  DeBuffer<T>::swap(void) {
  std::lock_guard<std::mutex> lock(m);
  d = (d == Direction::NORMAL) ? Direction::REVERSE : Direction::NORMAL;
  data.swap(backData);
}
#endif
