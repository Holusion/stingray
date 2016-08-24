#include "eventListener.hpp"

class Controller : public EventListener {
public:
  void update(){
    
  }
  virtual int getAxis() const{
    return 5;
  }
  virtual int getQuit() const{
    return 0;
  }

};

extern "C"{
  // the class factories
  extern EventListener* create() {
   return new Controller;
  }
  extern void destroy(EventListener* p) {
   delete p;
  }
}
