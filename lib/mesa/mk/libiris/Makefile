#	$OpenBSD: Makefile,v 1.6 2024/04/02 10:42:13 jsg Exp $

LIB=	iris

NOPROFILE=

SRCS=	i915_iris_batch.c \
	i915_iris_bufmgr.c \
	i915_iris_kmd_backend.c \
	xe_iris_batch.c \
	xe_iris_bufmgr.c \
	xe_iris_kmd_backend.c \
	iris_batch.c \
	iris_binder.c \
	iris_blit.c \
	iris_border_color.c \
	iris_bufmgr.c \
	iris_clear.c \
	iris_context.c \
	iris_draw.c \
	iris_fence.c \
	iris_fine_fence.c \
	iris_formats.c \
	iris_kmd_backend.c \
	iris_measure.c \
	iris_monitor.c \
	iris_perf.c \
	iris_performance_query.c \
	iris_pipe_control.c \
	iris_program.c \
	iris_program_cache.c \
	iris_resolve.c \
	iris_resource.c \
	iris_screen.c \
	iris_disk_cache.c \
	iris_utrace.c

i915_iris_batch.c:
	ln -s ${MESA_SRC}/src/gallium/drivers/iris/i915/iris_batch.c $@
i915_iris_bufmgr.c:
	ln -s ${MESA_SRC}/src/gallium/drivers/iris/i915/iris_bufmgr.c $@
i915_iris_kmd_backend.c:
	ln -s ${MESA_SRC}/src/gallium/drivers/iris/i915/iris_kmd_backend.c $@
xe_iris_batch.c:
	ln -s ${MESA_SRC}/src/gallium/drivers/iris/xe/iris_batch.c $@
xe_iris_bufmgr.c:
	ln -s ${MESA_SRC}/src/gallium/drivers/iris/xe/iris_bufmgr.c $@
xe_iris_kmd_backend.c:
	ln -s ${MESA_SRC}/src/gallium/drivers/iris/xe/iris_kmd_backend.c $@

.include "../Makefile.inc"

CFLAGS+=	${C_VIS_ARGS} ${C_SSE2_ARGS}
CPPFLAGS+=	-DGALLIUM_IRIS \
		-I${MESA_SRC}/src/gallium/drivers/iris \
		-I${MESA_SRC}/src/gallium/include \
		-I${MESA_SRC}/src/gallium/auxiliary \
		-I${MESA_SRC}/src/gallium/drivers \
		-I${MESA_SRC}/src/mesa \
		-I${MESA_SRC}/src/mapi \
		-I${MESA_SRC}/src/intel \
		-I${MESA_SRC}/src/compiler/nir \
		-I${MESA_SRC}/generated/src \
		-I${MESA_SRC}/generated/src/intel \
		-I${MESA_SRC}/generated/src/compiler \
		-I${MESA_SRC}/generated/src/compiler/nir

install:

obj: _xenocara_obj

.include <bsd.lib.mk>
.include <bsd.xorg.mk>

.PATH: ${MESA_SRC}/src/gallium/drivers/iris

CLEANFILES+=	i915_iris_batch.c \
		i915_iris_bufmgr.c \
		i915_iris_kmd_backend.c \
		xe_iris_batch.c \
		xe_iris_bufmgr.c \
		xe_iris_kmd_backend.c
