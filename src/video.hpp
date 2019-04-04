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
      decoder::DecoderContext  context;
      bool                     pause;
      int16_t                  speed;
      DeBuffer<VideoFrame*>   buffer;
      std::uint8_t             alpha;
      std::atomic<Video_State> state;
      int                      size;
    private:
      std::thread th;

    public:
      Video(const char* file,int width,int height, std::size_t start_frame_number = 0); //!< Can throw AVException
      ~Video();
      static void decode_loop (entities::Video* video);
      void alpha_add(std::uint8_t a){
        std::uint8_t res = this->alpha + a;
        DEBUG_LOG("Alpha Add to "<<std::to_string(res)<<std::endl);
        if(res < this->alpha){
          this->alpha = UINT8_MAX;
        }else{
          this->alpha = res;
        }
      }
      void alpha_sub(std::uint8_t a){
        std::uint8_t res = this->alpha - a;
        DEBUG_LOG("Alpha Sub to "<<std::to_string(res)<<std::endl);
        if(this->alpha < res){
          //Check for underflow
          this->alpha = 0;
        }else{
          this->alpha = res;
        }
      }
  };
}
#endif
