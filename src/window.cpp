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
  }else if (SDL_GetCurrentDisplayMode(display_index, &mode) != 0) {
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
  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
  SDL_GetWindowSize(m_window, &m_width, &m_height);

  m_displayFrame = SDL_CreateTexture(m_renderer,
      SDL_PIXELFORMAT_YV12,
      SDL_TEXTUREACCESS_STREAMING,
      m_width,
      m_height);

  /* Create black frame */
  lastFrame = av_frame_alloc();
  lastFrame->width = getWidth();
  lastFrame->height = getHeight();
  lastFrame->format = AV_PIX_FMT_YUVJ420P;
  avpicture_fill((AVPicture*)lastFrame, NULL, AV_PIX_FMT_YUVJ420P, lastFrame->width, lastFrame->height);
  int ret = av_image_alloc(lastFrame->data, lastFrame->linesize, lastFrame->width, lastFrame->height,
      AV_PIX_FMT_YUVJ420P, 32);
  if (ret < 0)
    throw  AVException(ret, "Could not allocate raw picture buffer");
  for (int j = 0; j < lastFrame->height; ++j) {
    for (int x = 0 ; x < lastFrame->width; ++x) {
      lastFrame->data[0][(j * lastFrame->linesize[0]) + x] = 0;
      lastFrame->data[1][(j / 2 * lastFrame->linesize[1]) + x / 2] = 128;
      lastFrame->data[2][(j / 2 * lastFrame->linesize[2]) + x / 2] = 128;
    }
  }
}

Window::~Window() {
  std::cout<<"SDL quit"<<std::endl;
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void  Window::draw(entities::Video& video) {

  static const SDL_Rect position {0, 0, m_width, m_height};
  int waitingTime;
  AVFrame*              frame;

  timer.setTargetSpeed(video.speed);
  for(int i=0;i<timer.getSkipCount();i++){
    if(video.buffer->size() < 2){
      DEBUG_LOG("Buffer too slow at "<<video.speed<<"fps"<<std::endl);
      break; //don't empty the buffer...
    }
    video.buffer->forward();
  }
  
  if(video.state == entities::in && video.buffer->size() > 0) {
    fadeIn(video, 255 * fadeMultiplier);
  } else if(video.state == entities::out && video.buffer->size() > 0) {
    fadeOut(video, 255 * fadeMultiplier);
  }
  waitingTime = timer.getWaitingTime(SDL_GetTicks());
  if(waitingTime == 0){
    DEBUG_LOG("Display too slow"<<std::endl);
  }else{
    SDL_Delay(1.0);
  }

  if(targetTime - currentTime > 0) {
    frame = lastFrame;
    if(video.buffer->size() && video.pause && video.state == entities::in && video.alpha <= 1) {
      frame = video.buffer->forward()->frame();
      lastFrame = frame;
    } else {
      currentTime += SDL_GetTicks() * (video.speed / 25.0); // 25 FPS
    }
  } else if ((video.buffer->size() > 0 && !video.pause)) {
    frame = video.buffer->forward()->frame();
    lastFrame = frame;
    currentTime = 0;
    targetTime = currentTime + waitingTime;
  }else{
    frame = lastFrame;
    currentTime = 0;
    targetTime = currentTime + waitingTime;
  }
  //! Draw
  SDL_RenderClear(m_renderer);

  SDL_UpdateYUVTexture(m_displayFrame,
      &position,
      frame->data[0],
      frame->linesize[0],
      frame->data[1],
      frame->linesize[1],
      frame->data[2],
      frame->linesize[2]);

  SDL_SetTextureBlendMode(m_displayFrame, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(m_displayFrame, video.alpha);

  SDL_RenderCopy(m_renderer, m_displayFrame, &position, &position);
  SDL_RenderPresent(m_renderer);

}

void Window::fadeIn(entities::Video& video, int delta) {
  if(video.alpha >= 255) {
    video.alpha = 255;
    video.state = entities::none;
  } else
    video.alpha += delta;
}

void Window::fadeOut(entities::Video& video, int delta) {
  if(video.alpha <= 0) {
    video.alpha = 0;
    video.state = entities::switch_state;
  }
  else
    video.alpha -= delta;
}
