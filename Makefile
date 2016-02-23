.PHONY: all clean realclean
.SUFFIXES: .asm .o
.INTERMEDIATE: x86/x86gen.exe

DRIVER ?= glide

all: lib/libGL.so.1.1

CC = gcc
HOST_CC = gcc
CFLAGS = -Wall -W -pedantic
CFLAGS += -O2
#CFLAGS += -g
#CFLAGS += -ffast-math -DFAST_MATH
CFLAGS += -I. -Iinclude -Idrivers
CFLAGS += -Wno-unused
LD = $(CC)
LDFLAGS =
LDLIBS = -lm
AS = nasm
ASFLAGS = -O6 -felf -D__linux__ -Ix86/ -Imain/

CORE_SOURCES = \
	util/cfg.c \
	util/pow.c \
	main/buffer.c \
	main/color.c \
	main/context.c \
	main/cull.c \
	main/depth.c \
	main/stencil.c \
	main/dlist.c \
	main/enable.c \
	main/get.c \
	main/glapi.c \
	main/glapic.c \
	main/light.c \
	main/matrix.c \
	main/texture.c \
	main/texstore.c \
	main/texcodec.c \
	main/texdef.c \
	main/fog.c \
	main/ext.c \
	main/pixel.c \
	main/raster.c \
	main/attrib.c \
	main/fake.c \
	main/legacy.c \
	util/alloc.c

TNL_SOURCES = \
	tnl/tnl.c \
	tnl/tnl_prim.c \
	tnl/tnl_pipeline.c \
	tnl/tnl_setup.c \
	tnl/imm_api.c \
	tnl/imm_array.c \
	tnl/imm_vertex.c \
	tnl/sav_api.c \
	tnl/sav_array.c \
	tnl/sav_vertex.c

ifeq ($(X86),1)
CFLAGS += -DX86
X86_SOURCES = \
	x86/x86.c \
	x86/cpuhard.c \
	x86/cpusoft.asm \
	x86/glapia.asm \
	x86/x86_vertex.asm \
	x86/x86_clip.asm \
	x86/k3d_mat.asm \
	x86/k3d_clip.asm \
	x86/k3d_misc.asm \
	x86/sse_vertex.asm \
	x86/sse_mat.asm \
	x86/sse_clip.asm \
	x86/sse_misc.asm

x86/x86.o: x86/x86chk.h
endif

include drivers/$(DRIVER)/config
DRIVER_SOURCES += \
	drivers/$(DRIVER)/glx.c

SOURCES = $(CORE_SOURCES) $(TNL_SOURCES) $(X86_SOURCES) $(DRIVER_SOURCES)

OBJECTS = $(addsuffix .o,$(basename $(SOURCES)))

.c.o:
	$(CC) -o $@ $(CFLAGS) -c $<
.asm.o:
	$(AS) -o $@ $(ASFLAGS) $<

lib/libGL.so.1.1: $(OBJECTS)
	$(LD) -o $@ -shared -Wl,-soname=libGL.so.1 $(LDFLAGS) $^ $(LDLIBS)

$(X86_SOURCES:.asm=.o): x86/x86.inc

x86/x86.inc: x86/x86gen.exe
	$< -o $@

x86/x86chk.h: x86/x86gen.exe
	$< -c -o $@

x86/x86gen.exe: x86/x86gen.c glinternal.h main/context.h main/matrix.h tnl/tnl.h x86/cpu.h
	$(HOST_CC) -o $@ $(CFLAGS) $<

clean::
	-$(RM) $(OBJECTS)
	-$(RM) x86/*.o
	-$(RM) x86/x86chk.h
	-$(RM) x86/x86.inc

realclean:: clean
	-$(RM) lib/libGL.so.1.1
