/* Copyright (C) 2016  Sebastien DUMETZ */
#ifndef  EVENT_MANAGER_HPP
#define  EVENT_MANAGER_HPP

#include  <SDL2/SDL.h>
#include  <thread>
#include <vector>
#include  "video.hpp"
#include "../config.h"
#ifdef ENABLE_MODULES
#include <regex>
#include <dirent.h>
#include <ltdl.h>
#endif
#include "eventListener.hpp"
#include  "events/SDLEvents.hpp"

#ifdef ENABLE_MODULES
typedef struct moduleStruct {
  EventListener* listener;
  destroy_t* destructor;
  update_t* update;
  lt_dlhandle handle;
} Module;
#endif
//! @class EventManager
//! @brief Where all events code are
class  EventManager {

  private:
    bool            quit; //!< Boolean which decide if the program run or not
    SDLEvents    events;
    std::vector<struct moduleStruct> modules;
    float fadeMultiplier = 0.007f;
    char* currentState = "fadeIn";
  public:
    EventManager();
    ~EventManager();
    bool  isEnd() { return quit; }
    //! @brief Update video data and more
    void         update(entities::Video& video);
    void exit(){ quit = true; };
    #ifdef ENABLE_MODULES
    void loadModules();
    void openModule(char* filename);
    void unloadModules();
    #endif
    #ifdef ENABLE_AUTOEXIT
    int autoexit_count = 0;
    int lastAxis=0;
    #endif

  private:
    void fadeIn(entities::Video& video);
    void fadeOut(entities::Video& video);
};
#endif
