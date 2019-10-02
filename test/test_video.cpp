#include  "test_video.hpp"
#include  <cmath>

namespace  entities {
  //FIXME pretty sure all av_frame_alloc() here are memory leaks...

  TEST_F(VideoTest, OpenVideo) {
    Video video(FIXTURES "/fixtures/goutput.mov",18,19);
    EXPECT_EQ(video.buffer.index(), 0);
  }
  TEST_F(VideoTest, OpenAtIndex) {
    Video video(FIXTURES "/fixtures/goutput.mov",18,19, 5);
    EXPECT_EQ(video.buffer.index(), 5);
  }
  TEST_F(VideoTest, Alpha_add){
    Video video(FIXTURES "/fixtures/goutput.mov",18,19);
    video.alpha = 5;
    video.alpha_add(100);
    EXPECT_EQ(105, video.alpha);
  }
  TEST_F(VideoTest, Alpha_add_clamp){
    Video video(FIXTURES "/fixtures/goutput.mov",18,19);
    video.alpha = 250;
    video.alpha_add(100);
    EXPECT_EQ(255, video.alpha);
  }
  TEST_F(VideoTest, Alpha_sub){
    Video video(FIXTURES "/fixtures/goutput.mov",18,19);
    video.alpha = 250;
    video.alpha_sub(100);
    EXPECT_EQ(150, video.alpha);
  }
      TEST_F(VideoTest, Alpha_sub_clamp){
    Video video(FIXTURES "/fixtures/goutput.mov",18,19);
    video.alpha = 50;
    video.alpha_sub(100);
    EXPECT_EQ(0, video.alpha);
  }
}
