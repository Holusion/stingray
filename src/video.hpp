#ifndef  VIDEO_HPP
#define  VIDEO_HPP

#include  "debuffer.hpp"
#include "video_frame.hpp"
#include "decoder_context.hpp"

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
