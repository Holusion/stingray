#ifndef  VIDEO_DECODER_TESTS_HPP
#define  VIDEO_DECODER_TESTS_HPP

#include  "gtest/gtest.h"
#include  "video_decoder.hpp"
#include  "decoder_context.hpp"
#include  "test_helpers/frames_compare.hpp"
#include  <iostream>
#include  <string>

namespace  decoder {

  class  VideoDecoderTest : public testing::Test {

    protected:

      DecoderContext  m_vcontext;
      VideoDecoder    m_decoder;

      // You can remove any or all of the following functions if its body
      // is empty.

      VideoDecoderTest() : m_vcontext(FIXTURES "/fixtures/goutput.mov",18,19),
                           m_decoder(VideoDecoder(m_vcontext)) {
      }

      // If the constructor and destructor are not enough for setting up
      // and cleaning up each test, you can define the following methods:

      virtual void SetUp() {
        // Code here will be called immediately after the constructor (right
        // before each test).
      }

      virtual void TearDown() {
        // Code here will be called immediately after each test (right
        // before the destructor).
      }
  };
}
#endif
