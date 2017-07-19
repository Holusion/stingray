#ifndef FRAMETIMER_HPP
#define FRAMETIMER_HPP
#include <inttypes.h>
#define MAXFPS 60
class FrameTimer{
    public:
        //Initializes variables
        FrameTimer();

        //The various clock actions
        int update(uint32_t ticks);
        void setTargetSpeed(int speed);
        int getWaitingTime(uint32_t ticks);
        int getSkipCount();
        //Gets the timer's time
    private:
      uint32_t last;
      uint32_t mean;
      uint32_t targetSpeed;
};
#endif
