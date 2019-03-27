#ifndef  VIDEO_TESTS_HPP
#define  VIDEO_TESTS_HPP

#include  "gtest/gtest.h"
#include  "video.hpp"
#include  <iostream>
#include  <string>

namespace  entities {

  class  VideoTest : public testing::Test {

    protected:

      // You can remove any or all of the following functions if its body
      // is empty.

      VideoTest() {
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
