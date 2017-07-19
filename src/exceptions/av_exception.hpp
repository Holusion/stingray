//! @file av_exception.hpp
//! @version 1.0
//! @date 21 December 2015
//! @author Sébastien Dumetz

#ifndef  AV_EXCEPTION_HPP
#define  AV_EXCEPTION_HPP

#include  <exception>
#include  <iostream>
#include  <string>
extern "C" {
  #include  <libavutil/error.h>
}
//! @class AVException
//! @brief Libav/Ffmpeg exception
class  AVException : public std::exception{

  private:
    char         m_log[100];
    std::string  log;

  public:
    AVException (int errnum,
                 std::string additional_infos="") {

      char errstr[100];

      if(additional_infos.size() > 0){
        additional_infos.insert(0,"in : ");
        additional_infos.append(", ");
      }
      log = additional_infos;
      av_strerror(AVERROR(errnum),errstr,100);
      log.append(std::to_string(errnum));
      log.append(" - ");
      log.append(errstr);
    }
    ~AVException(){
    };

  public:
    virtual const char* what() {
      return log.c_str();
    }
};
#endif
