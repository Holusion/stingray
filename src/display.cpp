#include "display.hpp"

using namespace core;

Display::Display(){
  if(SDL_Init(SDL_INIT_VIDEO) !=0)
    throw SDLException("Could not initialize SDL");

  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
}

Display::~Display(){
  std::cout<<"SDL quit"<<std::endl;
  SDL_Quit();
}

SDL_DisplayMode Display::getMode(){
  SDL_DisplayMode  mode;
  int display_count;
    if ((display_count = SDL_GetNumVideoDisplays()) < 1) {
    throw SDLException("SDL_GetNumVideoDisplays");
  }else if (SDL_GetCurrentDisplayMode(0, &mode) != 0) {
    throw SDLException("SDL_GetDisplayMode failed");
  }
  //SDL_Log("SDL_GetDisplayMode(0, 0, &mode):\t\t%i bpp\t%i x %i",SDL_BITSPERPIXEL(mode.format), mode.w, mode.h);
  return mode;
}

int  Display::getWidth() {
  return getMode().w;
}

int  Display::getHeight() {
  return getMode().h;
}

bool Display::active(){
  return currentSrc || nextSrc; //Force to bool
}

void Display::draw(){
  if(!active()){
    if(win)  win.reset();
    return;
  }else if(!win){
    win = std::unique_ptr<Window>(new Window(getMode()));
  }
  win->clear();
  #ifdef ENABLE_CROSSFADE
    if(this->nextSrc){
      if (0 == this->currentSrc->alpha || UINT8_MAX == this->nextSrc->alpha){
        this->currentSrc.swap(this->nextSrc); //Directly use the last-added element, skip every potential other video
        this->nextSrc.reset();
      }else{
        this->currentSrc->alpha_sub(CROSSFADE_SPEED);
        if(255/CROSSFADE_SPEED <= this->currentSrc->buffer.size()){
          this->currentSrc->state = entities::out; //Mark for end-of-decoding if we have enough frames
        }
      }
    }else if ( this->currentSrc->alpha < UINT8_MAX){
      this->currentSrc->alpha_add(CROSSFADE_SPEED);
    }
    win->draw(*this->currentSrc);
  #endif
  win->present();
}

void Display::setSource(std::shared_ptr<entities::Video>& new_source){
  #ifdef ENABLE_CROSSFADE
    if(this->currentSrc){
      this->nextSrc = new_source; //Reference the new source so it won't get deleted
    }else{
      this->currentSrc = new_source;
    }
  #else
    this->currentSrc = new_source;
  #endif
}



std::shared_ptr<entities::Video> Display::getSource(){
  if(this->currentSrc){
    return std::shared_ptr<entities::Video>(this->currentSrc);
  }else{
    return nullptr;
  }
  
}