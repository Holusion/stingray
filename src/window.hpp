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
namespace  core {

  class  Window{

    private:
      SDL_Window*           m_window;
      SDL_Renderer*         m_renderer;
      SDL_Texture*          m_displayFrame;
      AVFrame*              lastFrame;
      int            m_width, m_height;
      FrameTimer timer;
      float fadeMultiplier = 0.007f;
      int alpha;
    public:
      Window();
      ~Window();

    public:
      int  getWidth() { return m_width; }
      int  getHeight() { return m_height; }
      void        draw(entities::Video& video);
  };
}
#endif
