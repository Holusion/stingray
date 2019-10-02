#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include  "exceptions/sdl_exception.hpp"
#include  "video.hpp"
#include "window.hpp"
#include "debug.h"


namespace  core {
  #ifdef ENABLE_CROSSFADE
    #define CROSSFADE_SPEED 10
  #endif

  class  Display{
    private:
      std::shared_ptr<entities::Video> currentSrc;
      std::shared_ptr<entities::Video> nextSrc; //Always defined but only used when crossfade is enabled
      std::unique_ptr<Window> win;
    public:
      Display();
      ~Display();
      SDL_DisplayMode getMode();
      int  getWidth();
      int  getHeight();
      bool active();
      void setSource(std::shared_ptr<entities::Video>&);
      std::shared_ptr<entities::Video> getSource();
      void draw();
  };
}


#endif