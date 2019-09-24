#ifndef VTABLE_HPP
#define VTABLE_HPP
#include <systemd/sd-bus.h>

namespace vtable {
  using vtable_t = sd_bus_vtable;

  constexpr vtable_t start(decltype(vtable_t::flags) flags) {
    vtable_t v{};
    v.type = _SD_BUS_VTABLE_START;
    v.flags = flags;
    v.x.start = decltype(v.x.start){sizeof(vtable_t)};
    return v;
  }

  constexpr vtable_t end() {
    vtable_t v{};
    v.type = _SD_BUS_VTABLE_END;
    return v;
  }

  constexpr vtable_t method(const char* member, const char* signature, const char* result, sd_bus_message_handler_t handler, decltype(vtable_t::flags) flags = 0) {
    vtable_t v{};
    v.type = _SD_BUS_VTABLE_METHOD;
    v.flags = flags;
    v.x.method = decltype(v.x.method) {member, signature, result, handler, 0};
    return v;
  }
}
#endif