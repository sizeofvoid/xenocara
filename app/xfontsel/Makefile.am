#
#  Copyright 2005  Red Hat, Inc.
#
#  Permission to use, copy, modify, distribute, and sell this software and its
#  documentation for any purpose is hereby granted without fee, provided that
#  the above copyright notice appear in all copies and that both that
#  copyright notice and this permission notice appear in supporting
#  documentation, and that the name of Red Hat not be used in
#  advertising or publicity pertaining to distribution of the software without
#  specific, written prior permission.  Red Hat makes no
#  representations about the suitability of this software for any purpose.  It
#  is provided "as is" without express or implied warranty.
#
#  RED HAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
#  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
#  EVENT SHALL RED HAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
#  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
#  DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
#  TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
#  PERFORMANCE OF THIS SOFTWARE.

SUBDIRS = man
bin_PROGRAMS = xfontsel

AM_CPPFLAGS = -D_CONST_X_STRING
AM_CFLAGS = $(XFONTSEL_CFLAGS) $(CWARNFLAGS)
xfontsel_LDADD = $(XFONTSEL_LIBS)

xfontsel_SOURCES =	\
        ULabel.c \
        ULabel.h \
        ULabelP.h \
        xfontsel.c

# App default files
DISTCHECK_CONFIGURE_FLAGS = --with-appdefaultdir=\$${datadir}/X11/app-defaults

appdefaultdir = @appdefaultdir@

dist_appdefault_DATA = \
        app-defaults/XFontSel

MAINTAINERCLEANFILES = ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL

EXTRA_DIST = README.md

if USE_GETTEXT
AM_CPPFLAGS = -DLOCALEDIR='"$(LOCALEDIR)"'

noinst_DATA = xfontsel.po

xfontsel.po: $(xfontsel_SOURCES:%=$(srcdir)/%)
	$(AM_V_GEN)xgettext -d xfontsel -c -n $(xfontsel_SOURCES:%=$(srcdir)/%)

CLEANFILES = xfontsel.po
endif
