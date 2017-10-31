#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include "dbus.hpp"
#include <iostream>

using namespace dbus;

EventManager* DBus::manager = NULL;

DBus::DBus(EventManager* manager) {
  int r;
  DBus::manager = manager;

  r = sd_bus_open_user(&bus);
  if(r < 0) {
    std::cerr << "Failed to connect to system bus: " << (-r) << std::endl;
    return;
  }

  r = sd_bus_add_object_vtable(bus, &slot, "/com/stingray/Process", "com.stingray.Process", stingray_vtable, NULL);
  if(r < 0) {
    std::cerr << "Failed to issue method call: " << (-r) << std::endl;
    return;
  }

  r = sd_bus_request_name(bus, "com.stingray.Process", 0);
  if(r < 0) {
    std::cerr << "Failed to aquire service name: " << (-r) << std::endl;
    return;
  }
}

DBus::~DBus() {
  sd_bus_slot_unref(slot);
  sd_bus_unref(bus);
}

void DBus::update() {
  int r;
  r = sd_bus_process(bus, NULL);
  if(r < 0) {
    std::cerr << "Failed to process bus: " << (-r) << std::endl;
    return;
  }

  r = sd_bus_wait(bus, (uint64_t) -1);
  if(r < 0) {
    std::cerr << "Failed to wait on bus: " << (-r) << std::endl;
    return;
  }
}

int DBus::method_hello(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  int r = sd_bus_message_read(m, "");
  if(r < 0) {
    std::cerr << "Failed to parse parameters: " << (-r) << std::endl;
    return r;
  }

  return sd_bus_reply_method_return(m, "x", "Hello World") ;
}

/**
  fadeOut -> busctl --user call com.stingray.Process /com/stingray/Process com.stingray.Process VideoState x 1
  fadeIn ->  busctl --user call com.stingray.Process /com/stingray/Process com.stingray.Process VideoState x 0
*/
int DBus::method_video_state(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  char* videoState;
  int param;
  int r = sd_bus_message_read(m, "x", &param);
  if(r < 0) {
    std::cerr << "Failed to parse parameters: " << (-r) << std::endl;
    return r;
  }

  switch(param) {
    case 0:
      videoState = "fadeIn";
      break;
    case 1:
      videoState = "fadeOut";
      break;
    default:
      "";
  }

  cerr << "State changed to " << videoState << endl;
  manager->currentState = videoState;
  return sd_bus_reply_method_return(m, "x", r) ;
}

const sd_bus_vtable DBus::stingray_vtable[] = {
  vtable::start(0),
  vtable::method("Hello", "", "x", DBus::method_hello, SD_BUS_VTABLE_UNPRIVILEGED),
  vtable::method("VideoState", "x", "x", DBus::method_video_state, SD_BUS_VTABLE_UNPRIVILEGED),
  vtable::end()
};
