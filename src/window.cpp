#include  "window.hpp"


using namespace  std::chrono;
using namespace  core;



Window::Window() {
  if(SDL_Init(SDL_INIT_VIDEO))
    throw SDLException("Could not initialize SDL");


  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
  SDL_DisplayMode  mode;
  int display_count = 0, display_index = 0, mode_index = 0;
  if ((display_count = SDL_GetNumVideoDisplays()) < 1) {
    throw SDLException("SDL_GetNumVideoDisplays");
  }else if (SDL_GetDisplayMode(display_index, mode_index, &mode) != 0) {
    throw SDLException("SDL_GetDisplayMode failed");
  }
  SDL_Log("SDL_GetDisplayMode(0, 0, &mode):\t\t%i bpp\t%i x %i",SDL_BITSPERPIXEL(mode.format), mode.w, mode.h);
  //GetDesktopDisplayMode is relying on RandR extension, disabled by Xinerama
  //SDL_GetDesktopDisplayMode(0, &mode);
  std::cout<<"Detected desktop resolution : "<<mode.w<<"x"<<mode.h<<std::endl;
  /*! Create Window */
  m_window = SDL_CreateWindow(
      "Holusion Video Player",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      mode.w, mode.h,
       SDL_WINDOW_RESIZABLE);
  if(m_window == NULL)
    throw SDLException("SDL: could not create Window");
  //SDL_SetWindowGrab(m_window,SDL_TRUE);
  //! Create Renderer
  m_renderer = SDL_CreateRenderer(
      m_window,
      -1,
      SDL_RENDERER_ACCELERATED);
  if (m_renderer == NULL)
    throw SDLException( "SDL_CreateRenderer");

  SDL_GetWindowSize(m_window, &m_width, &m_height);

  m_displayFrame = SDL_CreateTexture(m_renderer,
      SDL_PIXELFORMAT_YV12,
      SDL_TEXTUREACCESS_STREAMING,
      m_width,
      m_height);

  /* Create black frame */
  lastFrame = nullptr;
}

Window::~Window() {
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void  Window::draw(entities::Video& video) {

  static const SDL_Rect position {0, 0, m_width, m_height};
  int waitingTime;
  char*              frame;

  timer.setTargetSpeed(video.speed);
  for(int i=0;i<timer.getSkipCount();i++){
    if(video.buffer->size() < 2){
      DEBUG_LOG("Buffer too slow at "<<video.speed<<"fps"<<std::endl);
      break; //don't empty the buffer...
    }
    video.buffer->forward();
  }
  waitingTime = timer.getWaitingTime(SDL_GetTicks());
  if(waitingTime == 0){
    DEBUG_LOG("Display too slow"<<std::endl);
  }else{
    SDL_Delay(waitingTime);
  }

  if ((video.buffer->size() > 0 && !video.pause)) {
    frame = video.buffer->forward()->frame();
    lastFrame = frame;
  }else if(lastFrame != nullptr){
    frame = lastFrame;
  }else{
    return;
  }
  //! Draw
  SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 0);
  SDL_RenderClear(m_renderer);
  #ifdef ENABLE_GSTREAMER
    SDL_Rect r{0, 0, m_width, m_height};
    SDL_UpdateTexture(m_displayFrame, &r, frame,
  		ROUND_UP_4(m_width));
  #else
    SDL_UpdateYUVTexture(m_displayFrame,
        &position,
        frame->data[0],
        frame->linesize[0],
        frame->data[1],
        frame->linesize[1],
        frame->data[2],
        frame->linesize[2]);
  #endif
  SDL_RenderCopy(m_renderer, m_displayFrame, &position, &position);
  SDL_RenderPresent(m_renderer);

}
