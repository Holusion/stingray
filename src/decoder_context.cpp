#include  "decoder_context.hpp"
#include  "window.hpp"

using namespace decoder;


DecoderContext::DecoderContext(const char* file,int width,int height) {
  AVCodec*  decoder;
  AVStream *video = NULL;
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

  /* find the video stream information */
  ret = av_find_best_stream(this->formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
  if (ret < 0) {
    throw AVException(ret,"Cannot find a video stream in the input file"); //! Couldn't find stream information
  }
  this->streamIndex = ret;
  video = this->formatCtx->streams[this->streamIndex];
  if (!(this->codecCtx = avcodec_alloc_context3(decoder))){
    throw AVException(AVERROR(ENOMEM),"not enough memory");
  }
  if (avcodec_parameters_to_context(this->codecCtx, video->codecpar) < 0)
      throw AVException(-1, "Failed to set context parameters");

  //this->codecCtx->get_format  = get_pix_format; 
  /*
  this->codecCtx = this->formatCtx->streams[this->streamIndex]->codec;
  if(this->codecCtx == NULL)
    throw AVException(0,"No stream found from avformat_find_stream_info"); //! Couldn't find or Didn't find a video stream
  //*/
  DEBUG_LOG("Decoder : "<<decoder->long_name<<std::endl);
 
  //If HWDEC setup fails or is disabled, this->codecCtx->hw_device_ctx == 0
  #if ENABLE_HWDEC
    // check for hardware decode
    // There may be multiple decoders so we might make a loop to search for the best one line in the FFMPEG example
    // https://www.ffmpeg.org/doxygen/4.1/hw_decode_8c-example.html#a54
    for (int i = 0;; i++) {
      const AVCodecHWConfig *config = avcodec_get_hw_config(decoder, i);
      if (!config) {
        DEBUG_LOG("Decoder "<< decoder->name<< " does not support device type " << std::endl);
        break;
      }else if(config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX){
        //Use the first available hardware decoder
        DEBUG_LOG("Found hwaccel decoder for : "<<av_hwdevice_get_type_name(config->device_type)<<std::endl);
        this->pix_fmt = config->pix_fmt; //used by video_decoder
        if ((ret = av_hwdevice_ctx_create(&this->codecCtx->hw_device_ctx, config->device_type,
                                        "/dev/dri/renderD128", NULL, 0)) < 0 || ! this->codecCtx->hw_device_ctx) {
          DEBUG_LOG("Failed to initialize hw_device_ctx : "<<std::to_string(ret));
        }
        break;
      }else{
        DEBUG_LOG("Not using decoder : "<< av_hwdevice_get_type_name(config->device_type)<<std::endl);
      }
    }
  #else
    DEBUG_LOG("not using hardware decoding"<<std::endl);
    this->pix_fmt = PIX_FMT_DEFAULT;
  #endif

  DEBUG_LOG("using pix format : "<<av_get_pix_fmt_name(this->pix_fmt)<<std::endl);
  //DEBUG_LOG("Supports "<<*fmts<<std::endl);
    //! Open codec
  ret = avcodec_open2(this->codecCtx, decoder, nullptr);
  if(ret !=0){
    throw AVException(ret,"avcodec_open2"); //! Could not open codec
  }
  
  this->fps = getFps();
  this->nbFrames = videoSize();
  this->nextFrame = 0;
  this->width = width;
  this->height = height;
  this->timeBase = av_q2d(this->formatCtx->streams[this->streamIndex]->time_base);
}

DecoderContext::~DecoderContext() {
  if(this->codecCtx->hw_device_ctx != NULL){
    av_buffer_unref(&this->codecCtx->hw_device_ctx);
  }
  
  avcodec_free_context(&this->codecCtx);
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


enum AVPixelFormat DecoderContext::get_pix_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts) {

  //if (pix_fmts[i] == PIX_FMT_DEFAULT) return pix_fmt[i];
  //Example from avcodec_default_get_format() in libavcodec/util.c
  //Returns the first non-hwaccel pix_fmt
  for(int i=0;;i++){
    if (pix_fmts[i] == AV_PIX_FMT_NONE) break;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmts[i]);
    if(desc->flags & AV_PIX_FMT_FLAG_HWACCEL){
      continue;
    }
    DEBUG_LOG("choosing pixel format : "<<av_get_pix_fmt_name(pix_fmts[i])<<std::endl);
    return pix_fmts[i];
  }
  return pix_fmts[0];
}
