### DBus service installation ###
data_desktopdir = $(datadir)/applications
data_desktop_in_files = com.stingray.desktop.in
data_desktop_data = $(data_desktop_in_files:.desktop.in=.desktop)

data_servicedir = $(datadir)/dbus-1/services
data_service_in_files = com.stingray.service.in
data_service_DATA = $(data_service_in_files:.service.in=.service)

$(data_service_DATA): $(data_service_in_files) Makefile
	@sed -e "s|\@bindir\@|$(bindir)|" $<> $@

EXTRA_DIST = 					\
	$(data_desktop_in_files)		\
	$(data_service_in_files)

CLEANFILES =	 			\
	$(data_desktop_DATA)		\
	$(data_service_DATA)

DISTCLEANFILES =	 		\
	$(data_desktop_DATA)		\
	$(data_service_DATA)
