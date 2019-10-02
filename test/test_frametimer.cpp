#include "test_frametimer.hpp"

TEST_F(FrameTimerTests, Update_Mean) {
  FrameTimer timer;
  timer.update(1);
  ASSERT_EQ(9,timer.update(10));
  ASSERT_EQ(10,timer.update(20));
  ASSERT_EQ(20,timer.update(40));
}
TEST_F(FrameTimerTests, First_Update_Doesnt_Break) {
  FrameTimer timer;
  ASSERT_EQ(0,timer.update(100));
}

TEST_F(FrameTimerTests, Calculate_Waiting_Time) {
  FrameTimer timer;
  timer.setTargetSpeed(25); //25 fps = should wait 40 ms
  timer.update(40);
  ASSERT_EQ(0,timer.getWaitingTime(80)); //return 0 while on sync
  ASSERT_EQ(10,timer.getWaitingTime(110)); //return time to wait before sync when ahead
  ASSERT_EQ(0,timer.getWaitingTime(160)); //expect display to take care of being back on time.
  ASSERT_EQ(0,timer.getWaitingTime(210)); //stick to 0 wait time when behind schedule
  ASSERT_EQ(0,timer.getWaitingTime(240)); //Expect display to catch up.
}
TEST_F(FrameTimerTests, Calculate_Waiting_Time_High_Framerates) {
  FrameTimer timer;
  timer.setTargetSpeed(60); //60 fps -> skip 1 frames out of 2 and display at 30 fps, aka 33ms/frame
  timer.update(33);
  ASSERT_EQ(0,timer.getWaitingTime(66)); //return 0 while on sync
}
TEST_F(FrameTimerTests, Have_Minimum_Refresh_Rate){
  FrameTimer timer;
  timer.setTargetSpeed(0);
  timer.update(40);
  ASSERT_EQ(0, timer.getWaitingTime(80)); //Ensure a minimum refresh rate of 30fps
}
/* DIsabled : too empirical
TEST_F(FrameTimerTests, Calculate_Skip_Frames) {
  FrameTimer timer;
  timer.setTargetSpeed(25);
  ASSERT_EQ(0,timer.getSkipCount()); //return 0 on low framerates
  timer.setTargetSpeed(60);
  ASSERT_EQ(1,timer.getSkipCount()); //skip 1 frame at 60fps
  timer.setTargetSpeed(90);
  ASSERT_EQ(2,timer.getSkipCount()); //skip 2 frames at 90fps
  timer.setTargetSpeed(120);
  ASSERT_EQ(3,timer.getSkipCount()); //skip 3 frames at 120fps
}
*/