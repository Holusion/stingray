#include  "decoder_context_gst.hpp"
#include  "window.hpp"

using namespace decoder;

/* This function is called when an error message is posted on the bus */
int DecoderContext::handle_message (GstBus *bus, GstMessage *msg, gpointer data) {
  GError *err;
  gchar *debug_info;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error (msg, &err, &debug_info);
      g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
      g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
      g_clear_error (&err);
      g_free (debug_info);
      //g_main_loop_quit (data->main_loop);
      break;
    case GST_MESSAGE_EOS:
      g_print ("End-Of-Stream reached.\n");
      break;
    case GST_MESSAGE_STATE_CHANGED: {
      GstState old_state, new_state, pending_state;
      gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
      std::cout<<"Play state change"<<std::endl;
    } break;
  }
  return 0;
}

DecoderContext::DecoderContext(const char* file,int width,int height) {
  loop = g_main_loop_new (NULL, FALSE);
  queryDuration = gst_query_new_duration (GST_FORMAT_TIME);
  //Create the pipeline
  pipeline = gst_pipeline_new("stingray-pipeline");
  GstElement* source = makeSource(file); //FIXME ensure absolute
  if (source == nullptr){
    std::cerr<<"Unable to open source"<<std::endl;
    return;
  }
  if (!gst_bin_add(GST_BIN(pipeline), source)) {
    std::cerr<<"Unable to add source element to the pipeline."<<std::endl;
    return;
  }

  sink = (GstAppSink*)gst_element_factory_make ("appsink", "app_sink");
  if (sink == nullptr) {
		std::cerr<<"Unable to create sink element.\n";
		return;
	}
  if (!gst_bin_add(GST_BIN(pipeline), (GstElement*)sink)) {
    std::cerr<<"Unable to add sink element to the pipeline.\n";
    return;
  }
  bus = gst_element_get_bus (pipeline);
  gst_bus_add_watch (bus, handle_message, nullptr);


  this->nextFrame = 0;
  this->fps = getFps();
  this->nbFrames = videoSize();
  this->width = width;
  this->height = height;
  //this->timeBase = av_q2d(this->formatCtx->streams[this->streamIndex]->time_base);
  std::cout<<"Done init"<<std::endl;
}

DecoderContext::~DecoderContext() {
  gst_object_unref (pipeline);
  gst_query_unref (queryDuration);
  gst_object_unref (bus);
  g_main_loop_unref (loop);
}

GstElement* DecoderContext::makeSource(const char* uri)
{
	GstElement* source;
  source = gst_element_factory_make ("uridecodebin",NULL);
  // Warning: the URI must be absolute.
  g_object_set(source, "uri", uri, NULL);

	return source;
}

void DecoderContext::mainLoop(){
  if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
		std::cerr<<"Unable to set the pipeline to the playing state.\n";
		return;
	}
  g_main_loop_run (loop);
}

int64_t  DecoderContext::getDurationNanoSec() {
  gint64 len;
  if (gst_element_query (pipeline, queryDuration)) {
    gst_query_parse_duration (queryDuration, NULL, &len);
  }else{
    std::cerr<<"Can't query media duration"<<std::endl;
    len = 5000000000;
  }
  return (int64_t)len;
}
double  DecoderContext::getDurationSec(){
  return ((double)getDurationNanoSec())/1000000000;
}

double  DecoderContext::getFps() {

  double fps = 0;

  //FIXME
  return (fps > 0) ? fps : 25;
}

int64_t  DecoderContext::videoSize() {

  int64_t  nbf = 0;
  //FIXME
  if (nbf == 0) {
    nbf = (int64_t)floor(getDurationSec() * getFps() + 0.5);
  }
  return nbf;
}
