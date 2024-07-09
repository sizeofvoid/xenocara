#	$OpenBSD: Makefile,v 1.11 2024/06/17 01:29:12 jsg Exp $

# gallium_drv_video
LIB=	r600_drv_video

NOPROFILE=

SRCS=	target.c

.include "../Makefile.inc"

CFLAGS+=	${C_VIS_ARGS}
CXXFLAGS+=	${CXX_VIS_ARGS}
CPPFLAGS+=	-I${MESA_SRC}/src/gallium/include \
		-I${MESA_SRC}/src/gallium/auxiliary \
		-I${MESA_SRC}/src/gallium/drivers \
		-I${MESA_SRC}/src/gallium/frontends/dri \
		-I${MESA_SRC}/src/gallium/frontends/va \
		-I${MESA_SRC}/src/gallium/winsys \
		-I${MESA_SRC}/src/mesa \
		-I${MESA_SRC}/src/mesa/drivers/dri/common \
		-I${MESA_SRC}/generated/src/util \
		-I${MESA_SRC}/generated/src/gallium/drivers \
		-DGALLIUM_SOFTPIPE

.if ${WITH_GALLIUM_R600} == "yes"
GALLIUM_DRIVERS+=	r600
CPPFLAGS+=	-DGALLIUM_R600
.endif

install:
	${INSTALL} ${INSTALL_COPY} ${INSTALL_STRIP} \
	    -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
	    lib${LIB}.so ${DESTDIR}${X11BASE}/lib/modules/dri/
.for driver in ${GALLIUM_DRIVERS}
	ln -f ${DESTDIR}${X11BASE}/lib/modules/dri/lib${LIB}.so \
	    ${DESTDIR}${X11BASE}/lib/modules/dri/${driver}_dri_.so
.endfor
	rm ${DESTDIR}${X11BASE}/lib/modules/dri/lib${LIB}.so

obj: _xenocara_obj

.include <bsd.lib.mk>
.include <bsd.xorg.mk>

SLIBS=	libdri \
	libmesa \
	libglsl \
	libglcpp \
	libmesa_util \
	libmesa_format \
	libmesa_util_sse41 \
	libblake3 \
	libmesa_util_c11 \
	libnir \
	libcompiler \
	libgalliumvl \
	libgallium \
	libpipe_loader_static \
	libloader \
	libxmlconfig \
	libws_null \
	libwsw \
	libswdri \
	libswkmsdri \
	libsoftpipe \
	libvtn

.if ${WITH_VAAPI} == "yes"
SLIBS+=	libva_st
.endif

.if ${WITH_SSE41} == "yes"
SLIBS+=	libmesa_sse41
.endif

.if ${WITH_GALLIUM_R300} == "yes" || ${WITH_GALLIUM_R600} == "yes" || \
    ${WITH_GALLIUM_RADEONSI} == "yes"
SLIBS+=	libradeonwinsys
.endif

.if ${WITH_GALLIUM_R600} == "yes"
SLIBS+=	libr600
.endif

LDADD+=		-Wl,--as-needed -Wl,--allow-shlib-undefined -Wl,--start-group
.for slib in ${SLIBS}
LDADD+=		${.CURDIR}/../${slib}/${__objdir}/${slib}.a
.endfor

LDADD+=		-L${.CURDIR}/../libglapi/${__objdir} -lglapi
LDADD+=		-lelf -lLLVM -lexpat -lz -lm -lpthread
LDADD+=		-L${X11BASE}/lib -ldrm

.if ${WITH_GALLIUM_R300} == "yes" || ${WITH_GALLIUM_R600} == "yes"
LDADD+=		-ldrm_radeon
.endif

LDADD+=		${BUILD_ID_SHA1} -Wl,--gc-sections \
		-Wl,--version-script ${MESA_SRC}/src/gallium/targets/dri/dri.sym
.if ${WITH_LD_DYNAMIC_LIST} == "yes"
LDADD+=		-Wl,--dynamic-list ${MESA_SRC}/src/gallium/targets/dri.dyn
.endif
LDADD+=		-Wl,--end-group

all:	lib${LIB}.so

lib${LIB}.so: ${SOBJS} ${DPADD}
	${CXX} -shared -Wl,-soname,lib${LIB}.so ${PICFLAG} -o ${.TARGET} \
	    `echo ${SOBJS} | tr ' ' '\n' | sort -R` ${LDADD}

CLEANFILES+= lib${LIB}.so

.PATH: ${MESA_SRC}/src/gallium/targets/va/
