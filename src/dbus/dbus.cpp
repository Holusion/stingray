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

  r = sd_bus_add_object_vtable(bus, &slot, "/com/stingray/Process", "org.freedesktop.Application", stingray_vtable, NULL);
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

  if(!DBus::manager->isEnd()) {
    r = sd_bus_wait(bus, (uint64_t) 1);
    if(r < 0) {
      std::cerr << "Failed to wait on bus: " << (-r) << std::endl;
      return;
    }
  }
}
int DBus::method_activate(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  return 0;
}


/**
changeVideo -> busctl --user call com.stingray.Process /com/stingray/Process org.freedesktop.Application Open asa{sv} 1 $(pwd)/test/fixtures/rolex.mov 1 desktop-startup-id s truc
*/
int DBus::method_open(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  char* videoState;
  int r = sd_bus_message_read(m, "as", 1, &videoState);
  if(r < 0) {
    std::cerr << "Failed to parse parameters: " << (-r) << std::endl;
    return r;
  }
  cout << "Open call :"<< videoState << endl;
  manager->nextVideo = (char*)videoState;
  manager->currentState = fade_out;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  return r;
}

int DBus::method_activate_action(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  return 0;
}

const sd_bus_vtable DBus::stingray_vtable[] = {
  vtable::start(0),
  vtable::method("Activate", "a{sv}", "", method_activate, SD_BUS_VTABLE_UNPRIVILEGED),
  vtable::method("Open", "asa{sv}", "", method_open, SD_BUS_VTABLE_UNPRIVILEGED),
  vtable::method("ActivateAction", "sava{sv}", "", method_activate_action, SD_BUS_VTABLE_UNPRIVILEGED),
  vtable::end()
};
