# Copyright (c) 2008, Oracle and/or its affiliates. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#

SUBDIRS=man
bin_PROGRAMS = showrgb

showrgb_SOURCES =  showrgb.c

if RGB_DB
bin_PROGRAMS += rgb

rgb_SOURCES = rgb.c

$(RGB_DB_FILES): rgb rgb.txt
	-rm -f $(RGB_DB_FILES)
	./rgb rgb < $(srcdir)/rgb.txt
endif

rgbdir = $(db_dir)
rgb_DATA = $(db_file).txt $(RGB_DB_FILES)
RGB_DB_DEFINES = -DRGB_DB=\"$(db_dir)/$(db_file)\"
AM_CFLAGS = $(RGB_CFLAGS) $(RGB_DB_DEFINES) $(CWARNFLAGS)

EXTRA_DIST= \
	README.md \
	rgb.txt \
	others/README \
	others/old-rgb.txt \
	others/raveling.txt \
	others/thomas.txt

MAINTAINERCLEANFILES = ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL
