/* Copyright (C) 2016  Sebastien DUMETZ */
#ifndef  DEBUFFER_HPP
#define  DEBUFFER_HPP

#include  <deque>
#include  <mutex>
#include <type_traits>

#include "../config.h"
#include "debug.h"
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
    std::size_t       current;
    const unsigned int count; //to know when we should round up
    buffer_t           data; //Is swapped when direction changes
    buffer_t           backData; // always contain data of !direction.
    Direction          d;
    const std::size_t  maxSize;
  public:
    //size parameter stands for "total video size" as in "count of frames"
    DeBuffer<T>(const std::size_t size, Direction d=Direction::NORMAL, std::size_t current=0) :
                                    m(),
                                    current(current % size),
                                    count(size),
                                    data(),
                                    backData(),
                                    d(d),
                                    maxSize((size < CAPACITY) ? size : CAPACITY) {};
    ~DeBuffer<T>(void){};

  public:
    std::size_t size() const{return data.size();}
    std::size_t sizeBackData() const{return backData.size();}
    std::size_t limit() const { return maxSize; }
    Direction   direction() const {return d; }
    std::size_t index() const {return current;}
    void  swap();
    bool write(T item, Direction d);
    bool write(T item);
    T forward();
    void forwardIndex();
    T &get(const unsigned int i);
    T getBackData(const unsigned int i);
    T &operator[] (const int); 
    void clean();
};

//Write with a direction, to allow for write-after-swap
template <class T>
bool DeBuffer<T>::write(T item, Direction write_d){
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
    return nullptr; //Would be better to handle the problem and :
    // Unlock mutex and block until e have a frame
  }

  backData.push_front(data.front());
  data.pop_front(); //Delete first after it has been moved
  if (maxSize < backData.size()) {
    backData.pop_back();
  }
  forwardIndex();
  return backData.front();
}

template <class T>
void DeBuffer<T>::forwardIndex(void){
  //Recalculate current index
  //Index start from 0 but frame count from 1. Thus we need to reset when ++current == count
  if (d == Direction::NORMAL){
    current = ++current % count;
  }else{
    current = (current == 0)? count-1:current-1;
  }
}
template <class T>
void  DeBuffer<T>::swap(void) {
  std::lock_guard<std::mutex> lock(m);
  d = (d == Direction::NORMAL) ? Direction::REVERSE : Direction::NORMAL;
  data.swap(backData);
}

template <class T>
T &DeBuffer<T>::get(const unsigned int i) {
  std::lock_guard<std::mutex> lock(m);
  return data[i];
}

template <class T>
T DeBuffer<T>::getBackData(const unsigned int i) {
  std::lock_guard<std::mutex> lock(m);
  return backData[i];
}

template <class T>
T &DeBuffer<T>::operator[](const int index) {
  return get(index);
}

template <class T>
void DeBuffer<T>::clean() {
  std::lock_guard<std::mutex> lock(m);
  buffer_t().swap(data);
  buffer_t().swap(backData);
}
#endif
