#ifndef _DECODE_HELPERS_HPP
#define _DECODE_HELPERS_HPP
#include  "gtest/gtest.h"
typedef struct {

  int y;
  int u;
  int v;
} yuvPix;
int isColor(const AVFrame& frame, yuvPix& color) {

  //Check a frame is uniformly of given color.
  int diff = 0;
  EXPECT_EQ(color.y, (int)frame.data[0][0]);
  EXPECT_EQ(color.u, (int)frame.data[1][0]);
  EXPECT_EQ(color.v, (int)frame.data[2][0]);

  //Check every subsequent pixel is OK
  for (int j = 0; j < frame.height; ++j) {
    for (int x = 0 ; x < frame.width; ++x) {
      ((color.y == (int)frame.data[0][(j * frame.linesize[0]) + x]) ? true : ++diff);
      ((color.u == (int)frame.data[1][(j / 2 * frame.linesize[1]) + x / 2]) ? true : ++diff);
      ((color.v == (int)frame.data[2][(j / 2 * frame.linesize[2]) + x / 2]) ? true : ++diff);
    }
  }
  EXPECT_EQ(0,diff);
  return diff;
}
int getFrameNumber(const AVFrame& frame){

  //Return frame's index in video. Also check the frame's integrity.
  yuvPix color;
  color.y = frame.data[0][0];
  color.u = frame.data[1][0];
  color.v = frame.data[2][0];

  if(isColor(frame,color) > 0){
    return -1;
  }else{
    return color.y/10-1;
  }
}

void expectValidFrame(const AVFrame& frame){
  EXPECT_EQ(AV_PIX_FMT_YUVJ420P,frame.format);
  EXPECT_EQ(frame.key_frame,1); //Every frame is a key frame
}


#endif
