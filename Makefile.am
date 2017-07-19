AUTOMAKE_OPTIONS = foreign
SUBDIRS = doc vendor src test

ACLOCAl_AMFLAGS = -I vendor/libltdl/m4 -I m4
### Binaries ###
bin_PROGRAMS = stingray
noinst_PROGRAMS = benchmark
check_PROGRAMS = test_stingray

TESTS = $(check_PROGRAMS)


### Main Program flags ###

stingray_CPPFLAGS = -Wall -Wextra -I$(top_srcdir)/vendor -I$(top_srcdir)/src \
                  $(SDL2_CFLAGS) $(avcodec_CFLAGS) $(avformat_CFLAGS) $(avutil_CFLAGS) $(LTDLINCL)

stingray_LDFLAGS	= -lpthread \
                  $(SDL2_LIBS) $(avcodec_LIBS) $(avformat_LIBS) $(avutil_LIBS) -export-dynamic
stingray_LDADD = $(LIBLTDL)
stingray_DEPENDENCIES = $(LTDLDEPS)

### Main Program sources files ###
stingray_SOURCES= src/launcher.cpp \
  src/events/SDLEvents.cpp \
  src/events/event_manager.cpp \
  src/frametimer.cpp \
  src/video.cpp \
  src/video_buffer.cpp \
  src/video_decoder.cpp \
  src/decoder_context.cpp \
  src/window.cpp

### Main Program header files ###
noinst_HEADERS =   src/constants.h \
  src/debug.h \
  src/exceptions/sdl_exception.hpp \
  src/exceptions/av_exception.hpp \
  src/exceptions/global_exception.hpp \
  src/events/SDLEvents.hpp \
  src/events/event_manager.hpp \
  src/events/eventListener.hpp \
  src/debuffer.hpp \
  src/video_frame.hpp \
  src/frametimer.hpp src/video.hpp src/video_buffer.hpp src/video_decoder.hpp src/decoder_context.hpp src/window.hpp

### Check Program flags ###
test_stingray_CPPFLAGS = -DFIXTURES=\"$(srcdir)/test\"  -DTESTS -I$(top_srcdir)/src -isystem $(top_srcdir)/vendor/gtest/include \
                       $(SDL2_CFLAGS) $(avcodec_CFLAGS) $(avformat_CFLAGS) $(avutil_CFLAGS)

test_stingray_LDFLAGS = $(avcodec_LIBS) $(avformat_LIBS) $(avutil_LIBS)
test_stingray_LDADD =  $(top_srcdir)/vendor/gtest/libgtest.a

### Check Program sources files ###
test_stingray_SOURCES = test/test_main.cpp test/test_frametimer.cpp test/test_video_decoder.cpp \
  src/frametimer.cpp src/video_buffer.cpp src/video_decoder.cpp src/decoder_context.cpp

### Check Program sources files ###
noinst_HEADERS += test/test_frametimer.hpp \
  test/test_video_decoder.hpp \
  test/test_helpers/frames_compare.hpp

### benchmark decoder ###
benchmark_SOURCES = test/benchmark_decodeSpeed.cpp \
  src/frametimer.cpp src/video_buffer.cpp src/video_decoder.cpp src/decoder_context.cpp src/video.cpp

benchmark_CPPFLAGS = -Wall -Wextra -I$(top_srcdir)/vendor -I$(top_srcdir)/src \
                  $(avcodec_CFLAGS) $(avformat_CFLAGS) $(avutil_CFLAGS)

benchmark_LDFLAGS	= -lpthread $(avcodec_LIBS) $(avformat_LIBS) $(avutil_LIBS) -export-dynamic
