/* Copyright (C) 2016  Sebastien DUMETZ */
#ifndef  DBUFFER_HPP
#define  DBUFFER_HPP

#include  <memory>

//! @namespace  memory
//! @brief Memory namespace
namespace  memory {

  //! @class DBuffer
  //! @brief Double buffer for general use
  template <template <typename T, class Alloc = std::allocator<T> > class Stock, class Unit>
  class  DBuffer {

    typedef Stock<Unit> buffer_t;

    protected:
      const std::size_t  m_maxSize;
      buffer_t           m_data;
      buffer_t           m_backData;

    public:
      DBuffer(const std::size_t maxSize) : m_maxSize(maxSize),
                                           m_data(),
                                           m_backData() {}
      virtual ~DBuffer() = default;

    public:
      //! @brief Getter on data
      const buffer_t&  current() { return m_data; }
      //! @brief Switch data and backData
      void  swap() { m_data.swap(m_backData); }
  };
}
#endif
