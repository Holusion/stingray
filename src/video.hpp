#ifndef  VIDEO_HPP
#define  VIDEO_HPP
extern "C" {
#include "../config.h"
}
#include  "debuffer.hpp"
#ifdef ENABLE_GSTREAMER
#include "decoder_context_gst.hpp"
#include "video_frame_gst.hpp"
#else
#include "decoder_context.hpp"
#include "video_frame.hpp"
#endif
//! @namespace entities
//! @brief Data namespace
namespace  entities {

  //! @class Video
  //! @brief Contain all data to play a video
  class Video {

    public:
      decoder::DecoderContext  context;
      bool                     pause;
      int16_t                  speed;
      DeBuffer<VideoFrame*>*   buffer;

    public:
      Video(const char* file,int width,int height); //!< Can throw AVException
      ~Video();
  };
}
#endif
