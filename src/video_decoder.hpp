#ifndef  VIDEO_DECODER_HPP
#define  VIDEO_DECODER_HPP

#ifdef  TESTS
#include  "gtest/gtest.h"
#endif

#include  <array>
#include  <cstddef>
#include  "exceptions/av_exception.hpp"
#include  <algorithm>
#include  "video.hpp"
#include  "constants.h"
//! @namespace  decoder
//! @brief  Decoder Video namespace
namespace  decoder {

  //! @class VideoDecoder
  //! @brief  Use Libav/Ffmpeg to decode some kind of video (format full keyframes)
  class  VideoDecoder {

    private:
      std::array<entities::VideoFrame*, DECODE_SIZE>  m_decodeArray;
      DecoderContext&                                 m_context;
    public:
      VideoDecoder(DecoderContext& context);
      ~VideoDecoder();

    private:
      AVFrame* tmpframe;

      //! @brief Update m_decodeArray after write
      void        updateVideoBuffer(std::size_t writes); //!< For now we always clean the full array

      void        seek(unsigned int position);

      //! @brief Decode the next frame in the video
      void        nextFrame(AVFrame* frame); //!< Can Throw AVException

      //! @brief Check where we need to decode from
      int         getDecodeIndex(const DeBuffer<entities::VideoFrame*>& buffer);


#ifdef  TESTS      //! @brief Some Google Tests

      //! @brief Unit Tests
      FRIEND_TEST(VideoDecoderTest, Decode_Normal);
      FRIEND_TEST(VideoDecoderTest, Decode_Reverse);
      FRIEND_TEST(VideoDecoderTest, Decode_Index);
      FRIEND_TEST(VideoDecoderTest, Decode_EOF);
      FRIEND_TEST(VideoDecoderTest, NextFrame);
      FRIEND_TEST(VideoDecoderTest, StartFrameNumber);
#endif

    public:
      //! @brief Decode one array of frame
      void  decode(unsigned int next);

      //! @brief Decode and write one array into the buffer
      void  decodeAndWrite(DeBuffer<entities::VideoFrame*>& buffer);
  };
}
#endif
