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
TEST_F(DeBufferTests, InitialIndex) {
  DeBuffer<int> buf(10, Direction::NORMAL, 2);
  ASSERT_EQ(2,buf.index()); //Always start at 0
}
TEST_F(DeBufferTests, InitialIndexLargerThanSize) {
  DeBuffer<int> buf(10, Direction::NORMAL, 15);
  ASSERT_EQ(5,buf.index()); //Always start at 0
}

TEST_F(DeBufferTests, SharedPointersIntegrationTest) {
  DeBuffer<std::shared_ptr<int>> *buf = new DeBuffer<std::shared_ptr<int>>(10);
  std::vector<std::shared_ptr<int>> v;
  for(int i = 0; i < 10; i++){
    v.push_back(std::shared_ptr<int>(new int(i)));
  }
  for ( std::vector<std::shared_ptr<int>>::iterator it = v.begin(); it != v.end(); ++it){
    buf->write(*it, Direction::NORMAL);
  }
  delete buf;
  for (auto const& it : v){
    ASSERT_EQ(1, it.use_count());
  }
}