#  Copyright 2005 Red Hat, Inc.
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
#

# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([libXvMC], [1.0.14],
	[https://gitlab.freedesktop.org/xorg/lib/libxvmc/-/issues], [libXvMC])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign dist-xz])

# Initialize libtool
LT_INIT

# Require X.Org macros 1.8 or later for AC_PROG_INSTALL
m4_ifndef([XORG_MACROS_VERSION],
          [m4_fatal([must install xorg-macros 1.8 or later before running autoconf/autogen])])
XORG_MACROS_VERSION(1.8)
XORG_DEFAULT_OPTIONS
XORG_CHECK_MALLOC_ZERO

# Obtain compiler/linker options for dependencies
PKG_CHECK_MODULES(XVMC, [x11 >= 1.6] xext xv xextproto videoproto)

# Checks for library functions.
AC_CHECK_FUNCS([shmat])

# Check to see if dlopen is in default libraries (like Solaris, which
# has it in libc), or if libdl is needed to get it.
AC_CHECK_FUNC([dlopen], [],
	AC_CHECK_LIB([dl], [dlopen], DLOPEN_LIBS="-ldl"))
AC_SUBST(DLOPEN_LIBS)

# Allow checking code with lint, sparse, etc.
XORG_WITH_LINT
LINT_FLAGS="${LINT_FLAGS} ${XVMC_CFLAGS}"

AC_CONFIG_FILES([Makefile
		include/Makefile
		include/X11/Makefile
		include/X11/extensions/Makefile
		src/Makefile
		wrapper/Makefile
		xvmc.pc
		xvmc-wrapper.pc])
AC_OUTPUT
