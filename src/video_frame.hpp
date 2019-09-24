//! @file video_frame.hpp
//! @version 1.0
//! @date 21 december 2015
//! @author Mathieu Van Nevel

#ifndef  VIDEO_FRAME_HPP
#define  VIDEO_FRAME_HPP

extern "C" {
  #include  <libavformat/avformat.h>
}
#ifdef  TESTS
#include  "gtest/gtest.h"
#endif
#include "debug.h"
//! @namespace  entities
//! @brief  Data namespace
namespace  entities {

  //! @class VideoFrame
  //! @brief To encapsulate one AVFrame
  class  VideoFrame {
    private:
      AVFrame*  m_frame;

#ifdef  TESTS

    FRIEND_TEST(CycleTests, VideoBuffer_Cycle);

    public:
      int  num;
#endif

    public:
      //static int alloc;
      VideoFrame();
      //! @brief Constructor from libav/ffmpeg
      VideoFrame(AVFrame* base);
      ~VideoFrame();
    public:
      AVFrame*  frame();
  };
}
#endif
