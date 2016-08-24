//! @file global_exception.hpp
//! @version 1.0
//! @date 21 december 2015
//! @author Mathieu Van Nevel

#ifndef  GLOBAL_EXCEPTION_HPP
#define  GLOBAL_EXCEPTION_HPP

#include  <exception>
#include  <iostream>
#include  <string>

//! @namespace core
//! @brief Core-System

  //! @class GlobalException
  //! @brief Base class of the Video-PLayer exception
class  GlobalException {

  private:
    std::string  m_name; //!< Exception Name
    std::string  m_where; //!< Position in files
    std::string  m_log;
  public:
    GlobalException (const std::string& name,
                     const std::string& where,
                     const std::string& log) : m_name(name), m_where(where), m_log(log) {
    }
    ~GlobalException() = default;

  public:
    virtual const char* what() {
      return ("\e[31m!!ERROR \e[32m<< " + m_name + " / "
              + m_where
              + " >>\e[0m ::  "
              + m_log).c_str();
    }
    static void  warning(const std::string& where,
                         const std::string& log) {

      std::cerr << "\e[33m**Warning \e[32m<< Video-Player / "
                   + where
                   + " >>\e[0m ::  "
                   + log
                << std::endl;
    }
};
#endif
