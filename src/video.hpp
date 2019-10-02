#ifndef  VIDEO_HPP
#define  VIDEO_HPP

#include  "debuffer.hpp"
#include "video_frame.hpp"
#include "decoder_context.hpp"
#include "video_decoder.hpp"
#include "debug.h"
#include  <thread>
#include <atomic>
#include "../config.h"
//! @namespace entities
//! @brief Data namespace
namespace  entities {

  enum Video_State {closed, in, out, play};
  //! @class Video
  //! @brief Contain all data to play a video
  class Video {

    public:
      const std::string        filename;
      decoder::DecoderContext  context;
      bool                     pause;
      int16_t                  speed;
      DeBuffer<std::shared_ptr<entities::VideoFrame>>   
                               buffer;
      std::uint8_t             alpha;
      std::atomic<Video_State> state;
      int                      size;
    private:
      std::thread th;

    public:
      Video(const std::string file,int width,int height, std::size_t start_frame_number = 0); //!< Can throw AVException
      ~Video();
      static void decode_loop (entities::Video* video);
      void alpha_add(std::uint8_t a);
      void alpha_sub(std::uint8_t a);
  };
}
#endif
