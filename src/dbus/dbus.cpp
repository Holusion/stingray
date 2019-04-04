
#include "dbus.hpp"

using namespace dbus;

std::mutex DBus::m;
std::string DBus::play_next_;
/**
This class creates a dbus interface for this application and listen the bus
to call the function called by the system
**/

DBus::DBus(EventManager* manager) : 
  quit(false)
  {
  int r;

  r = sd_bus_open_user(&bus);
  if(r < 0) {
    std::cerr << "Failed to connect to system bus: " << strerror(-r) << std::endl;
    return;
  }

  r = sd_bus_add_object_vtable(bus, &slot, "/com/stingray", "org.freedesktop.Application", stingray_vtable, NULL);
  if(r < 0) {
    std::cerr << "Failed to issue method call: " << strerror(-r) << std::endl;
    return;
  }

  r = sd_bus_request_name(bus, "com.stingray", 0);
  if(r < 0) {
    std::cerr << "Failed to aquire service name: " << strerror(-r) << std::endl;
    return;
  }
  th = std::thread(DBus::listen_loop, this);
}

DBus::~DBus() {
  quit = true;
  sd_bus_slot_unref(slot);
  sd_bus_unref(bus);
  th.join();
}

void DBus::update() {
  int r;
  r = sd_bus_process(bus, NULL);
  if(r < 0) {
    std::cerr << "Failed to process bus: " << strerror(-r) << std::endl;
    return;
  }

  if(r == 0 && !this->quit) {
    r = sd_bus_wait(bus, (uint64_t) 1);
    if(r < 0) {
      std::cerr << "Failed to wait on bus: " << strerror(-r) << std::endl;
      return;
    }
  }
}

std::string DBus::pop_play_next(){
  std::lock_guard<std::mutex> lock(DBus::m);
  std::string s = DBus::play_next_;
  DBus::play_next_ = "";
  return s;
}

std::string DBus::set_play_next(std::string str){
  std::lock_guard<std::mutex> lock(DBus::m);
  DBus::play_next_ = str;
}
std::string DBus::set_play_next(const char* str){
  std::lock_guard<std::mutex> lock(DBus::m);
  DBus::play_next_ = std::string(str);
}
int DBus::method_activate(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  return 0;
}


void DBus::listen_loop(dbus::DBus* bus) {

  while(!bus->quit) {
    bus->update();
  }
}

/**
changeVideo -> busctl --user call com.stingray /com/stingray org.freedesktop.Application Open asa{sv} 1 $(pwd)/test/fixtures/rolex.mov 1 desktop-startup-id s truc
*/
int DBus::method_open(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  char* videoState;
  int r = sd_bus_message_read(m, "as", 1, &videoState);
  if(r < 0) {
    std::cerr << "Failed to parse parameters: " << strerror(-r) << std::endl;
    return r;
  }
  set_play_next(videoState);
  r = sd_bus_reply_method_return(m, NULL);
  if(r < 0) {
    std::cerr << "Failed to send reply: " << strerror(-r) << std::endl;
    return r;
  }
  return 0;
}

int DBus::method_activate_action(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  return 0;
}

/**
This vtable is provided by the freedesktop dbus specification at :
https://standards.freedesktop.org/desktop-entry-spec/latest/ar01s08.html

<interface name='org.freedesktop.Application'>
  <method name='Activate'>
    <arg type='a{sv}' name='platform_data' direction='in'/>
  </method>
  <method name='Open'>
    <arg type='as' name='uris' direction='in'/>
    <arg type='a{sv}' name='platform_data' direction='in'/>
  </method>
  <method name='ActivateAction'>
    <arg type='s' name='action_name' direction='in'/>
    <arg type='av' name='parameter' direction='in'/>
    <arg type='a{sv}' name='platform_data' direction='in'/>
  </method>
</interface>

 - Activate : start the app without any file (does nothing beyond initalization)
 - Open : start the app with file(s) to opan. The array of strings is an array of URIs, in UTF-8.
 - ActivateAction : Custom actions as defined in the spec (https://standards.freedesktop.org/desktop-entry-spec/latest/ar01s11.html)
**/
const sd_bus_vtable DBus::stingray_vtable[] = {
  vtable::start(0),
  vtable::method("Activate", "a{sv}", "", method_activate, SD_BUS_VTABLE_UNPRIVILEGED),
  vtable::method("Open", "asa{sv}", "", method_open, SD_BUS_VTABLE_UNPRIVILEGED),
  vtable::method("ActivateAction", "sava{sv}", "", method_activate_action, SD_BUS_VTABLE_UNPRIVILEGED),
  vtable::end()
};
