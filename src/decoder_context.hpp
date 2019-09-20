#ifndef  DECODER_CONTEXT_HPP
#define  DECODER_CONTEXT_HPP

extern "C" {
  #include  <libavcodec/avcodec.h>
  #include  <libavformat/avformat.h>
  #include  <libavutil/pixdesc.h>
  #include <libavutil/hwcontext.h>
}
#include  "exceptions/global_exception.hpp"
#include  "exceptions/av_exception.hpp"
#define CALC_FFMPEG_VERSION(a,b,c) ( a<<16 | b<<8 | c )

#define PIX_FMT_DEFAULT AV_PIX_FMT_YUV420P;

//! @namespace decoder
//! @brief Decoder namespace
namespace decoder{

  //! @class DecodeContext
  //! @brief Contain data for video decode
  class  DecoderContext {

    private:
      static AVBufferRef *hw_device_ctx;
      static const constexpr double    EPS_ZERO = 0.000025;
      static enum AVPixelFormat get_pix_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);

    public:
      std::size_t      nextFrame; //!< Next frame that will get out of decoder
      std::size_t       nbFrames;
      double            fps;
      double            timeBase;
      int               width;
      int               height;
      //! @brief Some Libav/Ffmpeg data
      AVFormatContext*  formatCtx;
      AVCodecContext*   codecCtx;
      AVPixelFormat     pix_fmt;
      int               streamIndex;

    private:
      //! @brief Radian to Degree
      double                    r2d(AVRational r);
      double                    getDurationSec();
      double                    getFps();
      int64_t                   videoSize();

    public:
      DecoderContext(const char* file, int width, int height); //!< Can throw AVException
      ~DecoderContext();
  };
}
#endif
