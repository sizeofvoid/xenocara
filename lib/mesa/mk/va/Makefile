#	$OpenBSD: Makefile,v 1.5 2023/11/02 04:53:31 jsg Exp $

.include <bsd.xconf.mk>

LIB=	va

NOPROFILE=

SRCS=	buffer.c \
	config.c \
	context.c \
	display.c \
	image.c \
	picture.c \
	picture_av1.c \
	picture_av1_enc.c \
	picture_h264.c \
	picture_h264_enc.c \
	picture_hevc.c \
	picture_hevc_enc.c \
	picture_mjpeg.c \
	picture_mpeg12.c \
	picture_mpeg4.c \
	picture_vc1.c \
	picture_vp9.c \
	postproc.c \
	subpicture.c \
	surface.c

#.if ${XENOCARA_BUILD_DRI:L} == "yes"
#.endif

.include "../Makefile.inc"

CFLAGS+=	${C_VIS_ARGS}

# XXX keep in sync with libva
CPPFLAGS+=	-DVA_DRIVER_INIT_FUNC=__vaDriverInit_2_22 \

CPPFLAGS+=	-I${MESA_SRC}/src/gallium/frontends/va \
		-I${MESA_SRC}/src/gallium/include \
		-I${MESA_SRC}/src/gallium/auxiliary

install:

obj: _xenocara_obj

.include <bsd.lib.mk>
.include <bsd.xorg.mk>

.PATH: ${MESA_SRC}/src/gallium/frontends/va
