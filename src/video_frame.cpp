#include "video_frame.hpp"
using namespace  entities;
#ifdef VERBOSE_ALLOC
  int VideoFrame::alloc = 0;
#endif


VideoFrame::VideoFrame() : m_frame(av_frame_alloc()) {
  #ifdef VERBOSE_ALLOC
    alloc++;
    DEBUG_LOG("Allocated video frames "<<alloc<<std::endl);
  #endif
};

VideoFrame::VideoFrame(AVFrame* base) {
  #ifdef VERBOSE_ALLOC
    alloc++;
    DEBUG_LOG("Allocated video frames "<<alloc<<std::endl);
  #endif
  m_frame = av_frame_clone(base);
};

VideoFrame::~VideoFrame(){
  #ifdef VERBOSE_ALLOC
    alloc--;
    DEBUG_LOG("Destructor. Allocated frames : "<<alloc<<std::endl);
  #endif
  av_frame_free(&m_frame);
};

AVFrame*  VideoFrame::frame() { return m_frame; }