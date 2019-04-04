#ifndef DBUS_HPP
#define DBUS_HPP

#include "vtable.hpp"
#include  "events/event_manager.hpp"
#include "video.hpp"
#include "debug.h"
#include  <thread>
#include <atomic>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>

namespace dbus {
  class DBus {
    public:
      sd_bus_slot *slot = NULL;
      sd_bus *bus = NULL;
      DBus(EventManager* manager);
      ~DBus();
      void update();
      std::string pop_play_next();
    private:
      std::atomic<bool> quit;
      std::thread th;
      static std::mutex m;
      static std::string play_next_;

      static int method_activate(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
      static int method_open(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
      static int method_activate_action(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
      static const sd_bus_vtable stingray_vtable[];
      static void listen_loop(dbus::DBus* bus);

      static std::string set_play_next(std::string);
      static std::string set_play_next(const char*);
  };
}

#endif
