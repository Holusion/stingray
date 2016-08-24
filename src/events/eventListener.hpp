/* Copyright (C) 2016  Sebastien DUMETZ */
#ifndef EVENT_LISTENER_HPP
#define EVENT_LISTENER_HPP

class EventListener{

  public:
    EventListener(){}
    virtual ~EventListener(){}
    void update(){};
    virtual int getAxis() const = 0;
    virtual int getQuit() const = 0;
};
// the types of the class factories
typedef EventListener* create_t();
typedef void destroy_t(EventListener*);
typedef void update_t(EventListener*);
#endif
