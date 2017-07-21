#ifndef  VIDEO_DECODER_HPP
#define  VIDEO_DECODER_HPP
extern "C" {
  #include <gst/gst.h>
  #include <gst/app/gstappsink.h>
  #include "../config.h"
}
#include  <algorithm> //for std::reverse
#include "debuffer.hpp"
#include "decoder_context_gst.hpp"
#include "video_frame_gst.hpp"
//! @namespace  decoder
//! @brief  Decoder Video namespace
namespace  decoder {

  //! @class VideoDecoder
  //! @brief  Use Libav/Ffmpeg to decode some kind of video (format full keyframes)
  class  VideoDecoder {

    private:
      #ifdef USE_PULL_SAMPLE
      GstSample *tmpFrame;
      #else
      GstBuffer* tmpFrame;
      #endif

      std::array<entities::VideoFrame*, DECODE_SIZE>  m_decodeArray;
      DecoderContext& ctx;
    public:
      VideoDecoder(DecoderContext& context);
      ~VideoDecoder();

    private:

      void        seek(unsigned int position);

      //! @brief Decode the next frame in the video
      void        nextFrame(); //!< Can Throw AVException

      //! @brief Check where we need to decode from
      int         getDecodeIndex(DeBuffer<entities::VideoFrame*>& buffer);

    public:
      //! @brief Decode one array of frame
      void  decode(unsigned int next);

      //! @brief Decode and write one array into the buffer
      void  decodeAndWrite(DeBuffer<entities::VideoFrame*>& buffer);
  };
}
#endif
