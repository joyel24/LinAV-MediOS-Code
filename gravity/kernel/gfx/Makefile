include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

#specific OBJS def to select the right source files 
#according to the needed GFX mode: hardware / software plane

OBJS = osd.o

ifeq ($(GFX_MODE),DPLANE)
	OBJS += graphics_32_dplane.o graphics_8_dplane.o kgraphics_dplane.o
else
	OBJS += rendering.o gfxmemcpy.o graphics_32_gfx_ctx.o kgraphics_gfx_ctx.o swi_gfx_gfx_ctx.o
endif


SUBDIRS = fonts

all: subdirs $(OBJS)
	@cp -f *.o ../build

force: clean all        

clean: clean-subdirs 
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule