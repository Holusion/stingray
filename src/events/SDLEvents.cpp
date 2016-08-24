#include  "SDLEvents.hpp"

SDLEvents::SDLEvents():quit(false),axis(0) {
  #ifdef ENABLE_MOUSE
  SDL_ShowCursor(SDL_DISABLE);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  #endif
}

SDLEvents::~SDLEvents() {

}

void SDLEvents::update(){
  while(SDL_PollEvent(&event)){
    /* If a quit event has been sent */
    switch (event.type){
      case SDL_QUIT:
        /* Quit the application */
        quit = true;
        break;
      case SDL_KEYDOWN:
        setKey(event.key.keysym.sym,true);
        break;
      case SDL_KEYUP:
        setKey(event.key.keysym.sym,false);
        break;

  #ifdef ENABLE_MOUSE
      case SDL_MOUSEBUTTONDOWN:
        if(event.button.button == SDL_BUTTON_LEFT){
          clicked = true;
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if(event.button.button == SDL_BUTTON_LEFT){
          clicked = false;
        }
        break;
      case SDL_MOUSEMOTION:
        axis = event.motion.xrel;
        break;
  #endif
    }
  }
}
int SDLEvents::getQuit() const{return (quit)?1:0;}
int SDLEvents::getAxis() const{
  const int res = axis;
  return res;
}

void SDLEvents::setKey(int key,bool state){
  if(key == SDLK_LEFT && state){
    axis--;
  }else if(key == SDLK_RIGHT && state){
    axis++;
  }else if(key == SDLK_ESCAPE && state){
    quit = true;
  }
}
