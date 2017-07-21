#ifndef  DECODER_CONTEXT_HPP
#define  DECODER_CONTEXT_HPP

extern "C" {
  #include <gst/gst.h>
  #include <gst/app/gstappsink.h>
}
#include "../config.h"
#include  "exceptions/global_exception.hpp"

#define ROUND_UP_4 GST_ROUND_UP_4

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
      //double            timeBase;
      int               width;
      int               height;
      int               streamIndex;
      GstAppSink*       sink;
      GstElement*       pipeline;
      GstQuery *        queryDuration;
      GstBus *          bus;
      GMainLoop *loop;

    private:
      int64_t                    getDurationNanoSec();
      double                    getDurationSec();
      double                    getFps();
      int64_t                   videoSize();
      GstElement* makeSource(const char* uri);

    public:
      DecoderContext(const char* file, int width, int height); //!< Can throw AVException
      ~DecoderContext();
      static int handle_message (GstBus *bus, GstMessage *msg, gpointer data);
      void mainLoop();
  };
}
#endif
