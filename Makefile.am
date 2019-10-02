AUTOMAKE_OPTIONS = foreign
SUBDIRS = doc vendor src test data

ACLOCAl_AMFLAGS = -I vendor/libltdl/m4 -I m4
### Binaries ###
bin_PROGRAMS = stingray
noinst_PROGRAMS = benchmark
check_PROGRAMS = test_stingray

TESTS = $(check_PROGRAMS)


FFMPEG_CFLAGS = $(avcodec_CFLAGS) $(avformat_CFLAGS) $(avutil_CFLAGS)
FFMPEG_LIBS = $(avcodec_LIBS) $(avformat_LIBS) $(avutil_LIBS)
FFMPEG_ASSEMBLIES = ffmpeg/libavformat/libavformat.a \
                  ffmpeg/libavcodec/libavcodec.a \
                  ffmpeg/libavutil/libavutil.a \
                  ffmpeg/libswresample/libswresample.a

### Main Program flags ###


stingray_CPPFLAGS = -Wall -Wextra -I$(top_srcdir)/vendor -I$(top_srcdir)/ffmpeg -I$(top_srcdir)/src -g \
                  $(systemd_CFLAGS) $(SDL2_CFLAGS) $(FFMPEG_CFLAGS) \
                  $(LTDLINCL)

stingray_LDFLAGS	= -lpthread $(systemd_LIBS) $(SDL2_LIBS) $(FFMPEG_LIBS)

stingray_LDADD = $(LIBLTDL)
                  
stingray_DEPENDENCIES = $(LTDLDEPS)


### Main Program sources files ###
stingray_SOURCES= src/launcher.cpp \
  src/dbus/dbus.cpp \
  src/events/SDLEvents.cpp \
  src/events/event_manager.cpp \
  src/frametimer.cpp \
  src/video.cpp \
  src/video_decoder.cpp \
  src/decoder_context.cpp \
  src/video_frame.cpp \
  src/window.cpp \
  src/display.cpp

### Main Program header files ###
noinst_HEADERS =  src/debug.h \
  src/dbus/dbus.hpp \
  src/dbus/vtable.hpp \
  src/exceptions/sdl_exception.hpp \
  src/exceptions/av_exception.hpp \
  src/exceptions/global_exception.hpp \
  src/events/SDLEvents.hpp \
  src/events/event_manager.hpp \
  src/events/eventListener.hpp \
  src/debuffer.hpp \
  src/video_frame.hpp \
  src/frametimer.hpp src/video.hpp src/video_decoder.hpp src/decoder_context.hpp src/window.hpp src/display.hpp

### Check Program flags ###
test_stingray_CPPFLAGS = -DFIXTURES=\"$(srcdir)/test\"  -DTESTS -I$(top_srcdir)/src -isystem $(top_srcdir)/vendor/gtest/include \
                       $(SDL2_CFLAGS) $(FFMPEG_CFLAGS) -I$(top_srcdir)/ffmpeg

test_stingray_LDFLAGS =  -lpthread -lx264 -llzma -lm -lz $(FFMPEG_LIBS)
test_stingray_LDADD =  $(top_srcdir)/vendor/gtest/libgtest.a

### Check Program sources files ###
test_stingray_SOURCES = test/test_main.cpp \
  test/test_video.cpp test/test_debuffer.cpp test/test_video_decoder.cpp test/test_frametimer.cpp \
  src/video.cpp src/frametimer.cpp src/video_decoder.cpp  src/decoder_context.cpp src/video_frame.cpp

### Check Program sources files ###
noinst_HEADERS += test/test_frametimer.hpp \
  test/test_video.hpp \
  test/test_debuffer.hpp \
  test/test_video_decoder.hpp \
  test/test_helpers/frames_compare.hpp

### benchmark decoder ###
benchmark_SOURCES = test/benchmark_decodeSpeed.cpp \
  src/frametimer.cpp src/video_decoder.cpp src/decoder_context.cpp src/video.cpp src/video_frame.cpp

benchmark_CPPFLAGS = -Wall -Wextra $(FFMPEG_CFLAGS) \
                    -I$(top_srcdir)/vendor -I$(top_srcdir)/ffmpeg -I$(top_srcdir)/src

benchmark_LDFLAGS	= -lpthread -lpthread -lx264 -llzma -lm -lz $(x11_LIBS) $(FFMPEG_LIBS) -export-dynamic
