#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([stingray], [0.1.0], [s.dumetz@holusion.com])
AX_CXX_COMPILE_STDCXX(11, [noext]) # must be before AC_CONFIG_AUX_DIR
AC_CONFIG_AUX_DIR([build-aux]) # must be on top of LT macros
LT_INIT([dlopen])

LT_CONFIG_LTDL_DIR([vendor/libltdl])
LTDL_INIT([recursive])

AC_CONFIG_SRCDIR([src/launcher.cpp])
AC_CONFIG_HEADERS([config.h])


AC_CONFIG_MACRO_DIR([vendor/libltdl/m4])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_LIBLTDL_CONVENIENCE([vendor/libltdl])
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)
AC_PROG_LIBTOOL
#AC_CONFIG_SUBDIRS(vendor/libltdl)
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG(C++)


# Optional functionalities
# modules are default enabled
AC_ARG_ENABLE([modules],
    AS_HELP_STRING([--disable-modules], [Disable dynamic modules loading]))

AS_IF([test "x$enable_modules" != "xno"], [
  AC_DEFINE(ENABLE_MODULES,[1],[Enable dynamic modules loading])
])
# mouse control is default disabled
AC_ARG_ENABLE([mouse],
    AS_HELP_STRING([--enable-mouse], [Enable mouse control]))

AS_IF([test "x$enable_mouse" == "xyes"], [
  AC_DEFINE(ENABLE_MOUSE,[1],[Enable mouse control])
])

# autoexit (Default Disabled)
#Exit stingray after some seconds of inactivity
AC_ARG_ENABLE([autoexit],
    AS_HELP_STRING([--enable-autoexit], [Enable autoexit feature]))

AS_IF([test "x$enable_autoexit" == "xyes"], [
  AC_DEFINE(ENABLE_AUTOEXIT,[1],[Enable autoexit feature])
])

AC_ARG_ENABLE([gstreamer],
    AS_HELP_STRING([--enable-gstreamer], [Enable gstreamer for video decoding instead of ffmpeg]))

AM_CONDITIONAL([ENABLE_GSTREAMER], [test "x$enable_gstreamer" == "xyes"])
AS_IF([test "x$enable_gstreamer" == "xyes"], [
  AC_DEFINE(ENABLE_GSTREAMER,[1],[Enable gstreamer decoder])
  PKG_CHECK_MODULES(gstreamer, gstreamer-1.0)
  PKG_CHECK_MODULES(gstreamerbase, gstreamer-app-1.0)
  AC_CHECK_LIB([gstreamer-1.0], [gst_parse_launch])
  AC_CHECK_LIB([gstapp-1.0], [gst_app_sink_pull_buffer],[],[
    AC_CHECK_LIB([gstapp-1.0], [gst_app_sink_pull_sample],[
      AC_DEFINE(USE_PULL_SAMPLE,[1],[use old app sink pull_sample function])
    ],[
      AC_MSG_ERROR("app sink is required to decode with gstreamer. Install base plugins")
    ])

  ])
], [
  AC_MSG_NOTICE("using ffmpeg for video decoding")
])
# Checks for pkg-config pc files
PKG_CHECK_MODULES(SDL2, sdl2)
PKG_CHECK_MODULES(avcodec, libavcodec >= 54.3.0)
PKG_CHECK_MODULES(avformat, libavformat >= 54.3.0)
PKG_CHECK_MODULES(avutil, libavutil >= 52.3.0)


# Checks for libraries.
AC_CHECK_LIB([SDL2], [SDL_PollEvent])
AC_CHECK_LIB([avcodec], [avcodec_find_decoder])
AC_CHECK_LIB([avformat], [avformat_alloc_context])
AC_CHECK_LIB([avutil], [av_strerror])
AC_CHECK_LIB([pthread], [pthread_create])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h string.h unistd.h])
AC_CHECK_HEADER([ltdl.h],
    [AC_CHECK_LIB([ltdl], [lt_dladvise_init],
        [LIBLTDL=-lltdl], [LIBLTDL=])],
    [LIBLTDL=])
# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([floor strerror])



AC_CONFIG_FILES([ Makefile
                  doc/Makefile
                  src/Makefile
                  test/Makefile
                  vendor/Makefile
                  vendor/libltdl/Makefile
                ])

AC_OUTPUT
