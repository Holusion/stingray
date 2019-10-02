#include  "video_decoder.hpp"

using namespace  decoder;

VideoDecoder::VideoDecoder(DecoderContext& context): m_context(context) {

}

VideoDecoder::~VideoDecoder(){

}


//!
//! @brief Update decode buffer : move remains data at the start
//!
void  VideoDecoder::updateVideoBuffer(std::size_t  writes) {
  if(writes != DECODE_SIZE){
    std::cerr<<"Invalid write size : "<<writes<<std::endl;
  }
}

int VideoDecoder::getDecodeIndex(const DeBuffer<std::shared_ptr<entities::VideoFrame>>& buffer){
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

void  VideoDecoder::decodeAndWrite(DeBuffer<std::shared_ptr<entities::VideoFrame>> & buffer) {
  Direction  direction = buffer.direction();
  std::vector<std::shared_ptr<entities::VideoFrame>>::iterator it;
  //! Decode
  decode(getDecodeIndex(buffer));

  if (direction == Direction::REVERSE){
    std::reverse(decoded_vector.begin(), decoded_vector.end());
  }
  for(it = decoded_vector.begin(); it != decoded_vector.end(); ++it){
    if(!buffer.write(*it, direction)){
      DEBUG_LOG("OUT OF CAPACITY"<<std::endl);
      break;
    }
  }
  decoded_vector.erase(
    decoded_vector.begin(),
    it
  );
}

void  VideoDecoder::decode(unsigned int current) {

  //! Seek if we are not on the right frame
  if (m_context.nextFrame != current)
    seek(current);

  //! Fill decodeArray with VideoFrame
  while(decoded_vector.size() < DECODE_SIZE){

    decoded_vector.push_back(std::shared_ptr<entities::VideoFrame>(new entities::VideoFrame()));
    nextFrame(decoded_vector.back()->frame());
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
        //FIXME use av_hwframe_transfer_get_formats to compute possible source/dest formats
        if(m_context.codecCtx->hw_device_ctx != 0 && src_frame->format == m_context.pix_fmt){
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
