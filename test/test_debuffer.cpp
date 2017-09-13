#include "test_debuffer.hpp"

TEST_F(DeBufferTests, ForwardIndex) {
  DeBuffer<int> buf(10);
  ASSERT_EQ(0,buf.index()); //Always start at 0
  buf.forwardIndex();
  ASSERT_EQ(1,buf.index());
  for (int i=0; i< 9; i++){
    buf.forwardIndex();
  }
  ASSERT_EQ(0,buf.index());
}
TEST_F(DeBufferTests, ForwardIndexReverse) {
  DeBuffer<int> buf(10, Direction::REVERSE);
  ASSERT_EQ(0,buf.index()); //Always start at 0
  buf.forwardIndex();
  ASSERT_EQ(9,buf.index());
  for (int i=0; i< 9; i++){
    buf.forwardIndex();
  }
  ASSERT_EQ(0,buf.index());
}