//! @file av_exception.hpp
//! @version 1.0
//! @date 21 December 2015
//! @author Sébastien Dumetz

#ifndef  SDL_EXCEPTION_HPP
#define  SDL_EXCEPTION_HPP

#include  <exception>
#include  <iostream>
#include  <string>
#include  <SDL2/SDL.h>

//! @class SDLException
class  SDLException : public std::exception{

  private:
    char         m_log[100];
    std::string  log;

  public:
    SDLException (std::string additional_infos="") {

      if(additional_infos.size() > 0){
        additional_infos.insert(0,"in : ");
        additional_infos.append(", ");
      }
      log = additional_infos;
      log.append(SDL_GetError());
      SDL_ClearError();
    }
    ~SDLException(){
    };

  public:
    virtual const char* what() {
      return log.c_str();
    }
};
#endif
