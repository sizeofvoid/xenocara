noinst_LTLIBRARIES = libbsd.la

# No APM support.
APM_SOURCES = $(srcdir)/../shared/pm_noop.c

if FREEBSD_KLDLOAD
KMOD_SOURCES = bsd_kmod.c
else
KMOD_SOURCES = $(srcdir)/../shared/kmod_noop.c
endif

if AGP
AGP_SOURCES = bsd_agp.c
else
AGP_SOURCES = $(srcdir)/../shared/agp_noop.c
endif

if ALPHA_VIDEO
# Cheat here and piggyback other alpha bits on ALPHA_VIDEO.
ARCH_SOURCES = \
	alpha_video.c
endif

if ARM_VIDEO
ARCH_SOURCES = arm_video.c \
	$(srcdir)/../shared/ioperm_noop.c
endif

if ARM64_VIDEO
ARCH_SOURCES = arm64_video.c \
	$(srcdir)/../shared/ioperm_noop.c
endif

if HPPA_VIDEO
ARCH_SOURCES = hppa_video.c\
	$(srcdir)/../shared/ioperm_noop.c
endif

if I386_VIDEO
ARCH_SOURCES = i386_video.c
endif

if LUNA88K_VIDEO
ARCH_SOURCES = luna88k_video.c \
	$(srcdir)/../shared/ioperm_noop.c
endif 

if PPC_VIDEO
ARCH_SOURCES = ppc_video.c \
	$(srcdir)/../shared/ioperm_noop.c
endif

if RISCV64_VIDEO
ARCH_SOURCES = riscv64_video.c \
	$(srcdir)/../shared/ioperm_noop.c
endif

if SGI_VIDEO
ARCH_SOURCES = sgi_video.c \
	$(srcdir)/../shared/ioperm_noop.c
endif

if SPARC64_VIDEO
# Cheat here and piggyback other sparc64 bits on SPARC64_VIDEO.
ARCH_SOURCES = \
	sparc64_video.c \
	bsd_sbus.c \
	$(srcdir)/../shared/ioperm_noop.c
endif

# FIXME: NetBSD Aperture defines (configure.ac)
AM_CFLAGS = $(XORG_CFLAGS) $(DIX_CFLAGS)

AM_CPPFLAGS = $(XORG_INCS)

libbsd_la_SOURCES = \
	$(srcdir)/../shared/posix_tty.c \
	$(srcdir)/../shared/sigio.c \
	$(srcdir)/../shared/vidmem.c \
	bsd_VTsw.c \
	bsd_init.c \
	bsd_bell.c \
	$(ARCH_SOURCES) \
	$(AGP_SOURCES) \
	$(APM_SOURCES) \
	$(AXP_SOURCES) \
	$(DRI_SOURCES) \
	$(KMOD_SOURCES) \
	$(RES_SOURCES)

# FIXME: Add these files to the build as needed
EXTRA_DIST = \
	memrange.h
