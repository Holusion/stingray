#ifndef DBUS_HPP
#define DBUS_HPP

#include "vtable.hpp"
#include  "events/event_manager.hpp"
#include "video.hpp"

namespace dbus {
  class DBus {
    public:
      sd_bus_slot *slot = NULL;
      sd_bus *bus = NULL;

      DBus(EventManager* manager);
      ~DBus();
      void update();
      void wait();

    private:
      static EventManager* manager;
      static int method_activate(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
      static int method_open(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
      static int method_activate_action(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
      static const sd_bus_vtable stingray_vtable[];
  };
}

#endif
