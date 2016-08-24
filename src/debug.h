#ifndef DEBUG_H
#define DEBUG_H
#include  <stdio.h>

#ifdef DEBUG
  #define DEBUG_LOG(x) do { std::cerr << x; } while (0)
#else
  #define DEBUG_LOG(x)
#endif

#endif
