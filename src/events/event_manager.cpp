#include  "event_manager.hpp"
#include  <stdio.h>
#include  <string.h>
#include  <errno.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <fcntl.h>
#include  <assert.h>
#include  <linux/input.h>


EventManager::EventManager() :
    quit(false),
    events(){
  #ifdef ENABLE_MODULES
  loadModules();
  #endif
}

EventManager::~EventManager() {
  #ifdef ENABLE_MODULES
  unloadModules();
  #endif
  quit = true;
  //FIXME really no problem with threads close?
}

#ifdef ENABLE_MODULES
void EventManager::loadModules(){
  std::string s;
  DIR           *d;
  struct dirent *dir;
  if(lt_dlinit () != 0)
    std::cerr<<"LT_ERROR lt_dltinit : "<<lt_dlerror()<<std::endl;
  /* Set the module search path. */
  const char *modules_path= std::getenv("MODULES_PATH");
  if(modules_path == NULL){
    modules_path = "/etc/stingray/modules.d";
  }
  DEBUG_LOG("using modules path : "<<modules_path<<std::endl);
  if (lt_dlsetsearchpath (modules_path) != 0)
    std::cerr<<"LT_ERROR lt_dlsetsearchpath : "<<lt_dlerror()<<std::endl;
  else{
    d = opendir(modules_path);
    if (d){
      while ((dir = readdir(d)) != NULL){
        if( strstr(dir->d_name,".so")== dir->d_name+strlen(dir->d_name)-3
          || strstr(dir->d_name,".la")== dir->d_name+strlen(dir->d_name)-3 ){
          DEBUG_LOG("Open module : "<<dir->d_name<<std::endl);
          openModule(dir->d_name);
        }else{
          DEBUG_LOG("discarded "<<dir->d_name<<" for modules"<<std::endl);
        }
      }
      closedir(d);
    }
  }
  modules.shrink_to_fit();
}
void EventManager::openModule(char* filename){
  Module input_module;
  std::cout<<"Opening : "<<filename<<std::endl;
  input_module.handle = lt_dlopenext (filename);
  if(input_module.handle == NULL){
    std::cerr<<"LT_ERROR  lt_dlopenext (" << filename << ") : " <<lt_dlerror()<<std::endl;
    return;
  }
  create_t* create_module = (create_t*) lt_dlsym(input_module.handle, "create");
  const char* dlsym_error = lt_dlerror();
  if (dlsym_error) {
    std::cerr << "Cannot load symbol create: " << dlsym_error << '\n';
  }
  input_module.destructor = (destroy_t*) lt_dlsym(input_module.handle, "destroy");
  dlsym_error = lt_dlerror();
  if (dlsym_error) {
    std::cerr << "Cannot load symbol destroy: " << dlsym_error << '\n';
  }
  input_module.update = (update_t*) lt_dlsym(input_module.handle, "update");
  dlsym_error = lt_dlerror();
  if (dlsym_error) { //it's "normal" to not have update for some modules.
    input_module.update = NULL;
  }

  input_module.listener = create_module();
  modules.push_back(input_module);
  std::cout<<"Loaded "<<filename<<std::endl;

}

void EventManager::unloadModules(){
  Module module;
  while (!modules.empty())
  {
    module =modules.back();
    module.destructor(module.listener);
    if(lt_dlclose (module.handle) != 0)
      std::cerr << "Cannot close module: " << lt_dlerror() << '\n';
    modules.pop_back();
  }
  if (lt_dlexit () != 0)
    std::cerr<<"LT_ERROR lt_dlexit : "<<lt_dlerror()<<std::endl;
  std::cout<<"UNLOADED_MODULES"<<std::endl;
}
#endif

state_t EventManager::update(){
  state_t s = state_t{0, false};
  events.update();
  #ifdef ENABLE_MODULES
  for(unsigned i =0; i < modules.size(); i++) {
    if(modules[i].update != NULL){
      modules[i].update(modules[i].listener);
    }
    s.axis += modules[i].listener->getAxis();
    s.quit = (modules[i].listener->getQuit() != 0 || quit)?true:false; //prevent quit unsetting
    //currentState = modules[i].listener->getState();
  }
  #endif
  if( modules.size() == 0 ){
    s.axis = events.getAxis(); //keyboard is ignored if any other input is active.
  }
  s.quit = s.quit || (events.getQuit() != 0);
  #ifdef ENABLE_AUTOEXIT
  if (s.axis == lastAxis){
    autoexit_count++;
    if (autoexit_count > video.size){ //update is called on each frame (~60fps)
      std::cout<<"Automatic exit (Inactive)"<<std::endl;
      s.quit = true;
    }
  }else if (autoexit_count != 0){
    autoexit_count = 0;
  }
  lastAxis = s.axis;
  #endif
  /*
  if(s.quit){
    std::cout<<"Exit"<<std::endl;
  } //*/
  quit = s.quit;
  return s;
}

/* Copyright (C) 2016 Sebastien DUMETZ
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program; if not, see <http://www.gnu.org/licenses>.
 * Additional permission under GNU GPL version 3 section 7
 * If you modify this Program, or any covered work, by linking or combining it with modules, containing parts covered by the terms of non free licenses, the licensors of this Program grant you additional permission to convey the resulting work.
*/
