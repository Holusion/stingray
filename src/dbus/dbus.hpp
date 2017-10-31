#ifndef DBUS_HPP
#define DBUS_HPP

#include "vtable.hpp"
#include  "events/event_manager.hpp"

namespace dbus {
  class DBus {
    public:
      sd_bus_slot *slot = NULL;
      sd_bus *bus = NULL;

      DBus(EventManager* manager);
      ~DBus();
      void update();

    private:
      static EventManager* manager;
      static int method_video_state(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
      static const sd_bus_vtable stingray_vtable[];
  };
}

#endif
