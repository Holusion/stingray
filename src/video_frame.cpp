#include "video_frame.hpp"
using namespace  entities;
//int VideoFrame::alloc = 0;

VideoFrame::VideoFrame() : m_frame(nullptr) {
  //alloc++;
  //DEBUG_LOG("Allocated video frames "<<alloc<<std::endl);
};

VideoFrame::VideoFrame(AVFrame* base) {
  //alloc++;
  //DEBUG_LOG("Allocated video frames "<<alloc<<std::endl);
  m_frame = av_frame_clone(base);
  av_frame_unref(base);
};

VideoFrame::~VideoFrame(){
  //alloc--;
  //DEBUG_LOG("Destructor. Allocated frames : "<<alloc<<std::endl);
  if (m_frame != nullptr)
    av_frame_free(&m_frame);
};

AVFrame*  VideoFrame::frame() { return m_frame; }