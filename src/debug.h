#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
  #include <iostream>
  #define DEBUG_LOG(x) do { std::cerr << x; } while (0)
#else
  #define DEBUG_LOG(x)
#endif

#endif
