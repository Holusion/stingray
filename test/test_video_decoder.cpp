#include  "test_video_decoder.hpp"
#include  <cmath>

namespace  decoder {


  TEST_F(VideoDecoderTest, NextFrame) {

    AVFrame* frame = av_frame_alloc();

    for (int i = 0; i < 20; i++) {

      ASSERT_NO_THROW(m_decoder.nextFrame(frame));
      ASSERT_EQ(i, getFrameNumber(*frame));
    }
  }

  //
  // Decode the video one time in normal mode
  //
  TEST_F(VideoDecoderTest, Decode_Normal) {

    ASSERT_NO_THROW(m_decoder.decode(0));

    for (unsigned int i = 0; i < DECODE_SIZE; i++) {

      const AVFrame* frame = m_decoder.m_decodeArray[i]->frame();

      expectValidFrame(*frame);
      EXPECT_EQ(i, getFrameNumber(*frame));
    }
  }

  TEST_F(VideoDecoderTest, Decode_Index) {
    const int  start = 5;

    ASSERT_NO_THROW(m_decoder.decode(start));

    for (int i = 1; i < DECODE_SIZE; i++) {

      const AVFrame* frame = m_decoder.m_decodeArray[i]->frame();

      expectValidFrame(*frame);
      EXPECT_EQ((i + start)%20, getFrameNumber(*frame));
    }
  }

  TEST_F(VideoDecoderTest, Decode_EOF) {

    int count = 15;

    ASSERT_NO_THROW(m_decoder.decode(count));

    for (int i = 0; i < DECODE_SIZE; ++i) {

      const AVFrame* frame = m_decoder.m_decodeArray[i]->frame();

      expectValidFrame(*frame);
      EXPECT_EQ((count++) % 20, getFrameNumber(*frame));
    }
  }

  TEST_F(VideoDecoderTest, Decode_And_Write) {

    entities::VideoBuffer  buffer(20);

    ASSERT_NO_THROW(m_decoder.decodeAndWrite(buffer));

    for (int i = 0; i < 10; i++) {

      const AVFrame* frame = buffer.read()->frame();

      expectValidFrame(*frame);
      EXPECT_EQ(i, getFrameNumber(*frame));
    }
  }

}
