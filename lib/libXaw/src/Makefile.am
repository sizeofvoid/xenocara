lib_LTLIBRARIES =

COMMON_SOURCES = \
	Actions.c \
	AllWidgets.c \
	AsciiSink.c \
	AsciiSrc.c \
	AsciiText.c \
	Box.c \
	Command.c \
	Converters.c \
	Dialog.c \
	DisplayList.c \
	Form.c \
	Grip.c \
	Label.c \
	List.c \
	MenuButton.c \
	MultiSrc.c \
	MultiSink.c \
	OS.c \
	Paned.c \
	Panner.c \
	Pixmap.c \
	Porthole.c \
	Private.h \
	Repeater.c \
	Scrollbar.c \
	Simple.c \
	SimpleMenu.c \
	Sme.c \
	SmeBSB.c \
	SmeLine.c \
	StripChart.c \
	Text.c \
	TextSink.c \
	TextSrc.c \
	TextAction.c \
	TextPop.c \
	TextTr.c \
	Toggle.c \
	Tree.c \
	Vendor.c \
	Viewport.c \
	XawIm.c \
	XawInit.c \
	XawI18n.c \
	XawI18n.h

AM_CFLAGS = \
	$(CWARNFLAGS)

AM_CPPFLAGS = \
	-I${top_srcdir}/include \
	-DPROJECT_ROOT=\"$(prefix)\"

if BUILD_XAW6

lib_LTLIBRARIES += libXaw6.la

libXaw6_la_CPPFLAGS =			\
	$(AM_CPPFLAGS)

libXaw6_la_CFLAGS =			\
	$(AM_CFLAGS)			\
	-DOLDXAW			\
	$(XAW6_CFLAGS)

libXaw6_la_SOURCES = \
	$(COMMON_SOURCES)

libXaw6_la_LDFLAGS = -version-info 6:1:0 -no-undefined
libXaw6_la_LIBADD = $(XAW6_LIBS)

if !PLATFORM_WIN32
install-exec-hook::
if PLATFORM_DARWIN
	$(AM_V_at)rm -f $(DESTDIR)$(libdir)/libXaw.6.@LIBEXT@
	$(AM_V_GEN)(cd $(DESTDIR)$(libdir) && $(LN_S) libXaw6.6.@LIBEXT@ libXaw.6.@LIBEXT@)
else
	$(AM_V_at)rm -f $(DESTDIR)$(libdir)/libXaw.@LIBEXT@.6
	$(AM_V_GEN)(cd $(DESTDIR)$(libdir) && $(LN_S) libXaw6.@LIBEXT@.6 libXaw.@LIBEXT@.6)
endif

uninstall-local::
if PLATFORM_DARWIN
	$(AM_V_at)rm -f $(DESTDIR)$(libdir)/libXaw.6.@LIBEXT@
else
	$(AM_V_at)rm -f $(DESTDIR)$(libdir)/libXaw.@LIBEXT@.6
endif
endif

endif

if BUILD_XAW7

lib_LTLIBRARIES += libXaw7.la

libXaw7_la_CPPFLAGS =			\
	$(AM_CPPFLAGS)

libXaw7_la_CFLAGS =			\
	$(AM_CFLAGS)			\
	-DXAW7				\
	$(XAW7_CFLAGS)

libXaw7_la_SOURCES =			\
	$(COMMON_SOURCES)		\
	Tip.c

libXaw7_la_LDFLAGS = -version-info 7:0:0 -no-undefined
libXaw7_la_LIBADD = $(XAW7_LIBS)

if !PLATFORM_WIN32
install-exec-hook::
if PLATFORM_DARWIN
	$(AM_V_at)rm -f $(DESTDIR)$(libdir)/libXaw.7.@LIBEXT@
	$(AM_V_GEN)(cd $(DESTDIR)$(libdir) && $(LN_S) libXaw7.7.@LIBEXT@ libXaw.7.@LIBEXT@)
else
	$(AM_V_at)rm -f $(DESTDIR)$(libdir)/libXaw.@LIBEXT@.7
	$(AM_V_GEN)(cd $(DESTDIR)$(libdir) && $(LN_S) libXaw7.@LIBEXT@.7 libXaw.@LIBEXT@.7)
endif

uninstall-local::
if PLATFORM_DARWIN
	$(AM_V_at)rm -f $(DESTDIR)$(libdir)/libXaw.7.@LIBEXT@
else
	$(AM_V_at)rm -f $(DESTDIR)$(libdir)/libXaw.@LIBEXT@.7
endif
endif

endif
