#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include "dbus.hpp"
#include <iostream>

using namespace dbus;

DBus::DBus() {
  int r;

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
/*
sd_bus_message_handler_t DBus::method_hello(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  return (sd_bus_message_handler_t)0;
}
*/

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

static int method_hello(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  int r = sd_bus_message_read(m, "");
  if(r < 0) {
    std::cerr << "Failed to parse parameters: " << (-r) << std::endl;
    return r;
  }

  return sd_bus_reply_method_return(m, "x", "Hello World") ;
}
