#include  "decoder_context.hpp"
#include  "window.hpp"

using namespace decoder;

DecoderContext::DecoderContext(const char* file,int width,int height) {

  int ret;

  //! Register all formats and codecs
  av_register_all();

  this->formatCtx = avformat_alloc_context();


  //! Open video file
  ret = avformat_open_input(&this->formatCtx, file, nullptr, 0);
  if(ret!=0)
    throw AVException(ret,"avformat_open_input"); //! Couldn't open file

  //! Retrieve stream information
  ret = avformat_find_stream_info(this->formatCtx, nullptr);
  if(ret<0)
    throw AVException(ret,"avformat_find_stream_info"); //! Couldn't find stream information

  //! Find the first video stream because we don't need more
  for(unsigned int i = 0; i < this->formatCtx->nb_streams; ++i)
    if(this->formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
      this->streamIndex = i;
      this->codecCtx = this->formatCtx->streams[i]->codec;
      break;
    }
  if(this->codecCtx == nullptr)
    throw AVException(0,"No stream found from avformat_find_stream_info"); //! Couldn't find or Didn't find a video stream
  //! Find the decoder for the video stream
  AVCodec*  codec = avcodec_find_decoder(this->codecCtx->codec_id);
  if(codec == nullptr)
    throw AVException(0,"avcodec_find_decoder : codec not found for stream"); // Codec not found
  //! Copy context
  std::cout<<"Codec:"<<codec->name<<std::endl;
  if ((this->codecCtxOrig = avcodec_alloc_context3(codec)) == nullptr)
    throw AVException(0,"avcodec_alloc_context3 Failed");
  ret = avcodec_copy_context(this->codecCtxOrig, this->codecCtx);
  if(ret !=0){
    throw AVException(ret,"avcodec_copy_context"); //! Error copying codec context
  }
  //! Open codec
  ret = avcodec_open2(this->codecCtx, codec, nullptr);
  if(ret !=0){
    throw AVException(ret,"avcodec_open2"); //! Could not open codec
  }

  this->nextFrame = 0;
  this->fps = getFps();
  this->nbFrames = videoSize();
  this->width = width;
  this->height = height;
  this->timeBase = av_q2d(this->formatCtx->streams[this->streamIndex]->time_base);
}

DecoderContext::~DecoderContext() {
  avcodec_close(this->codecCtx);
  avcodec_free_context(&this->codecCtxOrig);

  //! Close the video file
  avformat_close_input(&this->formatCtx);
}
double  DecoderContext::r2d(AVRational r) {

  return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

double  DecoderContext::getDurationSec() {

  double sec = (double)this->formatCtx->duration / (double)AV_TIME_BASE;

  if (sec < EPS_ZERO) {
    sec = (double)this->formatCtx->streams[this->streamIndex]->duration
      * r2d(this->formatCtx->streams[this->streamIndex]->time_base);
  }

  if (sec < EPS_ZERO) {
    sec = (double)this->formatCtx->streams[this->streamIndex]->duration
      * r2d(this->formatCtx->streams[this->streamIndex]->time_base);
  }

  return sec;
}

double  DecoderContext::getFps() {

  double fps = 0;

#if LIBAVCODEC_BUILD >= CALC_FFMPEG_VERSION(54, 1, 0)
  fps = r2d(this->formatCtx->streams[this->streamIndex]->avg_frame_rate);
#else
  fps = r2d(this->formatCtx->streams[this->streamIndex]->r_frame_rate);
#endif

#if LIBAVFORMAT_BUILD >= CALC_FFMPEG_VERSION(52, 111, 0)
  if (fps < EPS_ZERO) {
    fps = r2d(this->formatCtx->streams[this->streamIndex]->avg_frame_rate);
  }
#endif

  if (fps < EPS_ZERO) {
    fps = 1.0 / r2d(this->formatCtx->streams[this->streamIndex]->codec->time_base);
  }

  return (fps > 0) ? fps : 25;
}

int64_t  DecoderContext::videoSize() {

  int64_t  nbf = this->formatCtx->streams[this->streamIndex]->nb_frames;

  if (nbf == 0) {
    nbf = (int64_t)floor(getDurationSec() * getFps() + 0.5);
  }
  return nbf;
}
