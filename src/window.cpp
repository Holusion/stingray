#include  "window.hpp"


using namespace  std::chrono;
using namespace  core;



Window::Window(SDL_DisplayMode  mode) :currentTime(0), targetTime(0){
  int display_count = 0, display_index = 0, mode_index = 0;
  
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
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (m_renderer == NULL)
    throw SDLException( "SDL_CreateRenderer");
  //SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_ADD);
  SDL_SetRenderDrawColor(m_renderer, 0x0, 0x0, 0x0, 0x0);
  SDL_GetWindowSize(m_window, &m_width, &m_height);
  position = SDL_Rect{0, 0, m_width, m_height};
  texture = SDL_CreateTexture(m_renderer,
      SDL_PIXELFORMAT_YV12,
      SDL_TEXTUREACCESS_STREAMING,
      m_width,
      m_height);
  if(texture == NULL){
    throw SDLException(SDL_GetError());
  }
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  
  /* Create black frame */
  blackFrame = lastFrame = av_frame_alloc();
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
  draw(lastFrame,255);
}

Window::~Window() {
   av_freep(&blackFrame->data);
  av_frame_free(&blackFrame);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
}

void Window::clear(){
  SDL_RenderClear(m_renderer);
}
void Window::present(){
  SDL_RenderPresent(m_renderer);
}

void Window::draw(AVFrame* frame, std::uint8_t opacity){
  int r = SDL_UpdateYUVTexture(texture,
      &position,
      frame->data[0],
      frame->linesize[0],
      frame->data[1],
      frame->linesize[1],
      frame->data[2],
      frame->linesize[2]);
  if (r !=0){
    DEBUG_LOG("Failed to update texture : "<<SDL_GetError()<<std::endl);
  }
  r = SDL_SetTextureAlphaMod(texture, opacity);
  if(r != 0){
    DEBUG_LOG("Alpha modulation, not supported on this texture"<<std::endl);
  }
  SDL_RenderCopy(m_renderer, texture, NULL, &position);
}

void  Window::draw(entities::Video& video) {
  int waitingTime;
  AVFrame*              frame;
  timer.setTargetSpeed(video.speed);
  for(int i=0;i<timer.getSkipCount();i++){
    if(video.buffer.size() < 2){
      DEBUG_LOG("Buffer too slow at "<<video.speed<<"fps"<<std::endl);
      break; //don't empty the buffer...
    }
    video.buffer.forward();
  }
  waitingTime = timer.getWaitingTime(SDL_GetTicks());
  if(waitingTime == 0){
    DEBUG_LOG("Display too slow"<<std::endl);
  }else if(waitingTime < 10){ //approximate granularity of the wait function
    SDL_Delay(1);
  }else{
    //DEBUG_LOG("DELAY "<<waitingTime<<std::endl);
    SDL_Delay(waitingTime - 10);
  }

  if ((0 < video.buffer.size() && !video.pause)) {
    frame = video.buffer.forward()->frame();
  }else{
    if(0 < video.buffer.size()) frame = video.buffer[0]->frame();
    else frame = lastFrame;
  }
  //! Draw
  draw(frame, video.alpha);
  lastFrame = frame;
}
