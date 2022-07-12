#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([stingray], [0.1.0], [s.dumetz@holusion.com])


AX_CXX_COMPILE_STDCXX(17, [noext])# must be before AC_CONFIG_AUX_DIR

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

# Checks for modules
PKG_CHECK_MODULES(SDL2, sdl2)
PKG_CHECK_MODULES(avcodec, libavcodec >= 58)
PKG_CHECK_MODULES(avformat, libavformat >= 58)
PKG_CHECK_MODULES(avutil, libavutil >= 56)
PKG_CHECK_MODULES(systemd, libsystemd)
PKG_CHECK_MODULES(x264, x264)
PKG_CHECK_MODULES(lzma, liblzma)

# Checks for libraries.
AC_CHECK_LIB([SDL2], [SDL_PollEvent])
AC_CHECK_LIB([avcodec], [avcodec_find_decoder])
AC_CHECK_LIB([avformat], [avformat_alloc_context])
AC_CHECK_LIB([avutil], [av_strerror])
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([systemd], [sd_bus_open_user])

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

###                       ###
# Optional functionalities  #
###                       ###

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

# autoexit (Default Disabled) - Exit stingray after some seconds of inactivity
AC_ARG_ENABLE([autoexit],
    AS_HELP_STRING([--enable-autoexit], [Enable autoexit feature]))
AS_IF([test "x$enable_autoexit" == "xyes"], [
  AC_DEFINE(ENABLE_AUTOEXIT,[1],[Enable autoexit feature])
])

# dbus (Default enable)
AC_ARG_ENABLE([dbus],
  AS_HELP_STRING([--disable-dbus], [Disable DBus]))
AS_IF([test "x$enable_dbus" != "xno"], [
  AC_DEFINE(ENABLE_DBUS,[1],[Enable DBus])
])

# Configure buffer capacity
AC_ARG_ENABLE([capacity],
    AS_HELP_STRING([--enable-capacity], [Set the capacity]))
AS_IF([test "$enable_capacity"], [
  AC_DEFINE_UNQUOTED(CAPACITY, $enable_capacity, [Set capacity])
], [
  AC_DEFINE_UNQUOTED(CAPACITY, 32, [Set capacity])
])

# Configure decode batch size
AC_ARG_ENABLE([batch],
    AS_HELP_STRING([--enable-batch], [Set the cbatch size for decoder]))
AS_IF([test "$enable_batch"], [
  AC_DEFINE_UNQUOTED(DECODE_SIZE, $enable_batch, [Set batch decode size])
], [
  AC_DEFINE_UNQUOTED(DECODE_SIZE, 8, [Set batch decode size])
])

# allow stingray to auto-start video at last known frame number (default disable) - unstable
AC_ARG_ENABLE([seamless],
    AS_HELP_STRING([--enable-seamless], [Enable seamless video loop from dbus Open Action]))
AS_IF([test "x$enable_seamless" == "xyes"], [
  AC_DEFINE(ENABLE_SEAMLESS,[1],[Enable seamless video loop from dbus Open Action])
])

# enable crossfade when switching video (default enable)
AC_ARG_ENABLE([crossfade],
    AS_HELP_STRING([--disable-crossfades], [Enable crossfade when switching video]))
AS_IF([test "x$enable_crossfade" != "xno"], [
  AC_DEFINE(ENABLE_CROSSFADE,[1],[Enable crossfade when switching video])
])

# hardware acceleration (Default enable)
AC_ARG_ENABLE([hwdec],
  AS_HELP_STRING([--disable-hwdec], [Disable Hardware decode]))
AS_IF([test "x$enable_hwdec" != "xno"], [
  AC_DEFINE(ENABLE_HWDEC,[1],[Enable Hardware Decode])
])

AC_CONFIG_FILES([ Makefile
                  doc/Makefile
                  src/Makefile
                  test/Makefile
                  vendor/Makefile
                  vendor/libltdl/Makefile
                  data/Makefile
                ])

AC_OUTPUT
