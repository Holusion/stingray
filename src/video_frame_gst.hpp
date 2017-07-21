//! @file video_frame.hpp
//! @version 1.0
//! @date 21 december 2015
//! @author Mathieu Van Nevel

#ifndef  VIDEO_FRAME_HPP
#define  VIDEO_FRAME_HPP

extern "C" {
  #include <gst/gst.h>
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
      GstBuffer*  buf;
      char *data;
      #ifdef USE_PULL_SAMPLE
      GstMapInfo *map;
      #endif
    public:
      //! @brief Constructor from libav/ffmpeg
      VideoFrame(GstBuffer* base) {
        buf = gst_buffer_copy (base);
        #ifdef USE_PULL_SAMPLE
        gst_buffer_map(buf, map, GST_MAP_READ);
        data = (char*)map->data;
        #else
        data = GST_BUFFER_DATA(buf);
        #endif
      }
      ~VideoFrame() {
        #ifdef USE_PULL_SAMPLE
        gst_buffer_unmap(buf, map);
        #endif
        gst_buffer_unref(buf);
      }

    public:
      char*  frame() {
        return data;
     }
  };
}
#endif
