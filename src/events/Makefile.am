# sample Makefile to compile a stub libary to be used with vswitch
pkglib_LTLIBRARIES = joystick_inputs.la

joystick_inputs_la_CPPFLAGS = -I$(top_srcdir)/vendor/libltdl $(INCLTDL)
joystick_inputs_la_LIBADD = $(LIBLTDL)
joystick_inputs_la_LDFLAGS = -module $(LIBLTDL)
joystick_inputs_la_SOURCES = stubController.cpp
