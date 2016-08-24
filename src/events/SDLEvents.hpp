#ifndef  SDLEVENTS_HPP
#define  SDLEVENTS_HPP


#include  <stdio.h>
#include <SDL2/SDL.h>
#include "../config.h"
#include  "../exceptions/global_exception.hpp"
#include "eventListener.hpp"
class  SDLEvents: public EventListener {
private:
    SDL_Event event;
    bool quit;
    int axis;
    bool kleft, kright;
    #ifdef ENABLE_MOUSE
    bool clicked = false;
    #endif
    void setKey(int key, bool state);
  public:
    SDLEvents();
    ~SDLEvents();
    void update();
    virtual int getAxis() const;
    virtual int getQuit() const;
};
#endif
