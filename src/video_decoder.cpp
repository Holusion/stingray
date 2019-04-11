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

int VideoDecoder::getDecodeIndex(const DeBuffer<entities::VideoFrame*>& buffer){
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
  for (std::size_t i=0; i <  m_decodeArray.size(); i++){
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
  AVFrame *src_frame = NULL;
  int frameFinished = 0, ret;
  AVPacket packet;
  frame->width = m_context.width;
  frame->height = m_context.height;
  frame->format = AV_PIX_FMT_YUV420P;
  frame->color_range = AVCOL_RANGE_JPEG;
  do {
    ret = av_read_frame(m_context.formatCtx, &packet);
    if(ret == AVERROR_EOF) {
      //! End Of File
      av_packet_unref(&packet);
      seek(0);
    } else if(ret < 0) {
      //! Decode Error
      av_packet_unref(&packet);
      throw AVException(ret, "av_read_frame");
    } else {
      if(packet.stream_index == m_context.streamIndex){
              //! Decode Frame
        src_frame = av_frame_alloc();
        if(!src_frame){
          throw AVException(AVERROR(ENOMEM), "Can't alloc frame");
        }

        ret = avcodec_send_packet(m_context.codecCtx, &packet);
        if(ret < 0){
          throw AVException(ret, "Error decoding packet");
        }

        ret = avcodec_receive_frame(m_context.codecCtx, src_frame);
        if (ret != 0) {
          av_packet_unref(&packet);
          av_frame_free(&src_frame);
          continue;
        } else if (ret < 0) {
          throw AVException(ret, "avcodec_receive_frame");
        }
        frameFinished = 1;
        //List available pix_fmt
        AVPixelFormat* fmts = NULL;
        /*
        DEBUG_LOG("Getting formats"<<std::endl);
        ret = av_hwframe_transfer_get_formats(src_frame->hw_frames_ctx, AV_HWFRAME_TRANSFER_DIRECTION_FROM, &fmts, 0);
        if(ret < 0){
          throw AVException(ret, "hwframe get formats");
        }
        for(int i=0; fmts[i] != AV_PIX_FMT_NONE; i++){
          DEBUG_LOG("Supports " << av_get_pix_fmt_name(fmts[i]) << "(N°"<<fmts[i]<<")"<<std::endl);
        }
        //*/
        if(src_frame->format == m_context.pix_fmt){
          /* retrieve data from GPU to CPU */
          if ((ret = av_hwframe_transfer_data(frame, src_frame, 0)) < 0) {
            throw AVException(ret, "Error transfering the data to system memory");
          }
        }else{
          av_frame_ref(frame, src_frame);
        }
        av_frame_free(&src_frame);
      }else{
        //! Invalid Stream Index
        //std::cout<<"Invalid string index #"<<std::to_string(packet.stream_index)<<" (Reading on stream #"<<std::to_string(m_context.streamIndex)<<std::endl;
        // happens mainly when there is an audio stream
        av_packet_unref(&packet);
        //throw AVException(0, "invalid stream index");
      }
    }
  } while(!frameFinished);

  av_packet_unref(&packet);
  ++m_context.nextFrame;
}

void  VideoDecoder::seek(unsigned int target){

  double  realTarget = (double)target / m_context.fps / m_context.timeBase;
  int     error;
  DEBUG_LOG("SEEK to"<<std::to_string(target)<<std::endl);
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
