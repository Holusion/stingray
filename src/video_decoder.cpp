#include  "video_decoder.hpp"

using namespace  decoder;

VideoDecoder::VideoDecoder(DecoderContext& context): m_context(context) {
  tmpframe = av_frame_alloc();
  for (unsigned int i = 0; i < DECODE_SIZE; ++i)
    m_decodeArray[i] = nullptr;
}

VideoDecoder::~VideoDecoder(){
  av_frame_free(&tmpframe);
}


//!
//! @brief Update decode buffer : move remains data at the start
//!
void  VideoDecoder::updateVideoBuffer(std::size_t  writes) {
  if(writes != DECODE_SIZE){
    std::cerr<<"Invalid write size : "<<writes<<std::endl;
  }
}

int VideoDecoder::getDecodeIndex(DeBuffer<entities::VideoFrame*>& buffer){
  int          decodeFrom;

  if (buffer.direction() == Direction::NORMAL){
    decodeFrom = buffer.index() + buffer.size();
  }else{
    decodeFrom = buffer.index() - buffer.size() - DECODE_SIZE;
  }

  while (decodeFrom >= (int)m_context.nbFrames){
    decodeFrom -= m_context.nbFrames;
  }
  while (decodeFrom < 0 ){
    decodeFrom += m_context.nbFrames;
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

  //! Seek if we are not on the right frame
  if (m_context.nextFrame != current)
    seek(current);

  //! Fill decodeArray with VideoFrame
  for (unsigned int i = 0; i < DECODE_SIZE; ++i) {

    nextFrame(tmpframe);
    m_decodeArray[i] = new entities::VideoFrame(tmpframe);
  }
}

void VideoDecoder::nextFrame(AVFrame* frame) {

  int frameFinished = 0, ret;
  AVPacket packet;
  frame->width = m_context.width;
  frame->height = m_context.height;
  frame->format = AV_PIX_FMT_YUVJ420P;

  do {
    ret = av_read_frame(m_context.formatCtx, &packet);

    if(ret == AVERROR_EOF) {
      //! End Of File
      av_free_packet(&packet);
      seek(0);
    } else if(ret < 0) {
      //! Decode Error
      av_free_packet(&packet);
      throw AVException(ret, "av_read_frame");
    }
    else {
      //! Decode Frame
      if(packet.stream_index == m_context.streamIndex){
        ret = avcodec_decode_video2(m_context.codecCtx, frame, &frameFinished, &packet);
        if(ret<0){
          throw AVException(ret, "avcodec_decode_video2");
        }
      }else{
        //! Invalid Stream Index
        av_free_packet(&packet);
        throw AVException(0, "invalid stream index");
      }
    }
  } while(!frameFinished);

  av_free_packet(&packet);
  ++m_context.nextFrame;
}

void  VideoDecoder::seek(unsigned int target){

  double  realTarget = (double)target / m_context.fps / m_context.timeBase;
  int     error;
  std::cout <<"Seek to : "<<target<<std::endl;
  //! av_format_seek_file is working, unlike av_format_seek_frame
  if ((error = avformat_seek_file(m_context.formatCtx,
                         m_context.streamIndex,
                         INT64_MIN,
                         realTarget,
                         INT64_MAX,
                         AVSEEK_FLAG_ANY)) < 0)
    throw AVException(error, "unhandled fail on av_seek_frame");

  m_context.nextFrame = target;
  avcodec_flush_buffers(m_context.codecCtx);
}

void  VideoDecoder::seekBack(unsigned int x) {

  unsigned int target;

  if (m_context.nextFrame < x)
    target = m_context.nbFrames - (x - m_context.nextFrame);
  else
    target = m_context.nextFrame - x;
  seek(target);
}
