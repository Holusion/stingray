#ifndef  VIDEO_HPP
#define  VIDEO_HPP

#include  "debuffer.hpp"
#include "video_frame.hpp"
#include "decoder_context.hpp"

//! @namespace entities
//! @brief Data namespace
namespace  entities {

  enum Video_State {none, in, out, switch_state, not_play};
  //! @class Video
  //! @brief Contain all data to play a video
  class Video {

    public:
      decoder::DecoderContext  context;
      bool                     pause;
      int16_t                  speed;
      DeBuffer<VideoFrame*>*   buffer;
      int                      alpha;
      Video_State              state;
      int                      size;

    public:
      Video(const char* file,int width,int height); //!< Can throw AVException
      ~Video();
  };
}
#endif
