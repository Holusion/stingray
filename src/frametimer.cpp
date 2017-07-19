#include "frametimer.hpp"

FrameTimer::FrameTimer(){
  mean = last = targetSpeed = 0;
}
// take the video speed (in fps). return the number of frames we should skip
int FrameTimer::update(uint32_t ticks){
  if(last != 0){
    mean = mean*0+(ticks-last)*1;
  }
  last = ticks;
  return (int)mean;
}
void FrameTimer::setTargetSpeed(int speed){
  targetSpeed = (uint32_t)speed;
}

int FrameTimer::getWaitingTime(uint32_t ticks){
  int waitTimer;
  update(ticks);
  if(targetSpeed != 0){
    waitTimer = 1000*(getSkipCount()+1)/targetSpeed - mean;
  }else{ //default to 30 fps
    waitTimer = 1000*(getSkipCount()+1)/MAXFPS - mean;
  }
  last += waitTimer;
  return (waitTimer>=0)?waitTimer:0;
}
int FrameTimer::getSkipCount(){
  return (targetSpeed>MAXFPS)?targetSpeed/MAXFPS-1:0; //30 is the reference framerate here
}
