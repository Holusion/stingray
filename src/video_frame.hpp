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
      VideoFrame() : m_frame(nullptr) {};
      //! @brief Constructor from libav/ffmpeg
      VideoFrame(AVFrame* base) {

        m_frame = av_frame_clone(base);
        av_frame_unref(base);
      }
      ~VideoFrame() {

        if (m_frame != nullptr)
          av_frame_free(&m_frame);
      }

    public:
      AVFrame*  frame() { return m_frame; }
  };
}
#endif
