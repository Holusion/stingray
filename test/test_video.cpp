#include  "test_video.hpp"
#include  <cmath>

namespace  entities {
  //FIXME pretty sure all av_frame_alloc() here are memory leaks...

  TEST_F(VideoTest, OpenVideo) {
    Video video(FIXTURES "/fixtures/goutput.mov",18,19);
    EXPECT_EQ(video.buffer->index(), 0);
  }
    TEST_F(VideoTest, OpenAtIndex) {
    Video video(FIXTURES "/fixtures/goutput.mov",18,19, 5);
    EXPECT_EQ(video.buffer->index(), 5);
  }
}
