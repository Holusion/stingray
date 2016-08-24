#ifndef  DECODER_CONTEXT_HPP
#define  DECODER_CONTEXT_HPP

extern "C" {
  #include  <libavcodec/avcodec.h>
  #include  <libavformat/avformat.h>
}
#include  "exceptions/global_exception.hpp"
#include  "exceptions/av_exception.hpp"
#define CALC_FFMPEG_VERSION(a,b,c) ( a<<16 | b<<8 | c )

//! @namespace decoder
//! @brief Decoder namespace
namespace decoder{

  //! @class DecodeContext
  //! @brief Contain data for video decode
  class  DecoderContext {

    private:
      static const constexpr double    EPS_ZERO = 0.000025;

    public:
      unsigned int      nextFrame; //!< Next frame that will get out of decoder
      std::size_t       nbFrames;
      double            fps;
      double            timeBase;
      int               width;
      int               height;
      //! @brief Some Libav/Ffmpeg data
      AVFormatContext*  formatCtx;
      AVCodecContext*   codecCtxOrig;
      AVCodecContext*   codecCtx;
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
