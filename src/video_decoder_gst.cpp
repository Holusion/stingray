#include  "video_decoder_gst.hpp"

using namespace  decoder;

VideoDecoder::VideoDecoder(DecoderContext& context): ctx(context) {

}
VideoDecoder::~VideoDecoder(){
}



int VideoDecoder::getDecodeIndex(DeBuffer<entities::VideoFrame*>& buffer){
  int          decodeFrom;

  if (buffer.direction() == Direction::NORMAL){
    decodeFrom = buffer.index() + buffer.size();
  }else{
    decodeFrom = buffer.index() - buffer.size() - DECODE_SIZE;
  }

  while (decodeFrom >= (int)ctx.nbFrames){
    decodeFrom -= ctx.nbFrames;
  }
  while (decodeFrom < 0 ){
    decodeFrom += ctx.nbFrames;
  }

  return decodeFrom;
}

void  VideoDecoder::decodeAndWrite(DeBuffer<entities::VideoFrame*> & buffer) {
  Direction  direction = buffer.direction();

  //! Decode
  decode(getDecodeIndex(buffer));
  if (direction == Direction::REVERSE){
    std::reverse(m_decodeArray.begin(), m_decodeArray.end());
  }
  //! Update decodeArray
  for (int i=0; i <  m_decodeArray.size(); i++){
    if (!buffer.write(m_decodeArray[i], direction)){
      std::cout<<"Out of capacity"<<std::endl;
    }
  }

}

void  VideoDecoder::decode(unsigned int current) {
  std::cout<<"Decoding"<<std::endl;
  //! Seek if we are not on the right frame
  if (ctx.nextFrame != current){
    seek(current);
    std::cout<<"Seeked"<<std::endl;
  }

  //! Fill decodeArray with VideoFrame
  for (unsigned int i = 0; i < DECODE_SIZE; ++i) {

    nextFrame();
    std::cout<<"got a frame"<<std::endl;
    if (tmpFrame == NULL){
      std::cout<<"NULL frame"<<std::endl;
      m_decodeArray[i] = NULL;
      continue;
    }
    #ifdef USE_PULL_SAMPLE
    m_decodeArray[i] = new entities::VideoFrame(gst_sample_get_buffer (tmpFrame));
    gst_sample_unref(tmpFrame);
    #else
    m_decodeArray[i] = new entities::VideoFrame(tmpFrame);
    gst_buffer_unref(tmpFrame);
    #endif
  }
}

void VideoDecoder::nextFrame() {
  //is gst_app_sink_pull_buffer in newer versions
  #ifdef USE_PULL_SAMPLE
  tmpFrame = gst_app_sink_pull_sample(ctx.sink);
  #else
  tmpFrame = gst_app_sink_pull_buffer(ctx.sink);
  #endif
}

void  VideoDecoder::seek(unsigned int target){

  gint64 realTarget = target*1000000000 / ctx.fps ;
  int     error;

  //! av_format_seek_file is working, unlike av_format_seek_frame
  if (!gst_element_seek (ctx.pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                         GST_SEEK_TYPE_SET, realTarget,
                         GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
    std::cerr<<"seek failed"<<std::endl;
  }
  ctx.nextFrame = target;
}
