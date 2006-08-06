include ../../Makefile.conf

#GCCFLAGS = -Wall -mlong-calls  -nostdlib -fno-builtin  -mcpu=arm7tdmi -Os

ASFLAGS  += -I./include -I../../include -I. 

GCCFLAGS += -I./include -I../../include -I. 


TARGET = aoboy

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

LDFLAGS =  -nostdlib --emit-relocs -L../../lib -lmedios -T$(TARGET_DEVICE)_aoboy.lds

LIBS = ../../lib/libmedios.a

SUBDIRS_CLEAN = build

all: builddir $(LIBS) $(OBJS)
	@cp -f $(OBJS) build
	@$(LD)  build/*.o ../../lib/*.o -o $(TARGET).med $(LDFLAGS) -Map avboy.map
	@rm -f .elf *.gdb *.elf2flt
        
$(LIBS):
	make -C ../../ lib_medios
        
stdalone: GCCFLAGS += -DBUILD_STDALONE
stdalone: builddir $(OBJS)
	@cp -f $(OBJS) build

builddir:
	@mkdir -p build

include ../../default.rule