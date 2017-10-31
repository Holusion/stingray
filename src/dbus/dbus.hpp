#ifndef DBUS_HPP
#define DBUS_HPP

#include "vtable.hpp"

namespace dbus {
  class DBus {
    public:
      sd_bus_slot *slot = NULL;
      sd_bus *bus = NULL;

      DBus();
      ~DBus();
      void update();
    };
}

static int method_hello(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) ;

static const sd_bus_vtable stingray_vtable[] = {
  vtable::start(0),
  vtable::method("Hello", "", "x", method_hello, SD_BUS_VTABLE_UNPRIVILEGED),
  vtable::end()
};
#endif
