#ifndef  WINDOW_HPP
#define  WINDOW_HPP
#include <SDL2/SDL.h>
#include  <thread>

extern "C" {
#include  <libavutil/imgutils.h>
}
#include  "exceptions/sdl_exception.hpp"
#include  "exceptions/av_exception.hpp"
#include  "video.hpp"
#include "frametimer.hpp"
#include "debug.h"
#include  "../config.h"

namespace  core {
  #ifdef ENABLE_CROSSFADE
    #define CROSSFADE_SPEED 10
  #endif

  class  Window{

    private:
      SDL_Window*           m_window;
      SDL_Renderer*         m_renderer;
      SDL_Texture* texture;
      AVFrame*              blackFrame;
      AVFrame*              lastFrame;
      int            m_width, m_height;
      bool shown;
      SDL_Rect position;
      FrameTimer timer;
      int targetTime;
      int currentTime;
    public:
      Window(SDL_DisplayMode);
      ~Window();
    public:
      int  getWidth() { return m_width; }
      int  getHeight() { return m_height; }
      void draw(entities::Video& video);
      void draw(AVFrame* frame, std::uint8_t opacity);
      void hide();
      void clear();
      void present();
  };

}
#endif
