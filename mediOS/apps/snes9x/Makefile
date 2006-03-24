include ../../Makefile.conf

ASFLAGS  += -I../../include -I. -Icpu

GCCFLAGS += -I../../include -I. -O3


TARGET = snes9x

SRC = $(wildcard *.c)
SRCCPP = $(wildcard spc/libspc/*.cpp) $(wildcard *.cpp)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRCCPP:%.cpp=%.o) $(SRC2:%.S=%.o)

LIBS = ../../lib/libmedios.a

GCCFLAGS += -Ispc/libspc -DHOST_LITTLE_ENDIAN -DSPC_PLAYER=1 -DNO_INLINE_SET_GET
# -DSPC700_SHUTDOWN
LDFLAGS =  -nostdlib --emit-relocs -L../../lib -lmedios -T$(TARGET_DEVICE).lds

all: builddir $(LIBS) $(OBJS)
	@cp -f $(OBJS) build
	@$(LD) -b binary dspcode/snes9x_dsp.out -b elf32-littlearm \
	 build/*.o ../../lib/*.o -o $(TARGET).med $(LDFLAGS) -Map snes9x.map
	@rm -f .elf *.gdb *.elf2flt
        

stdalone: GCCFLAGS += -DBUILD_STDALONE
stdalone: builddir $(OBJS)
	@cp -f $(OBJS) build
        
$(LIBS):
	make -C ../../kernel lib_medios 
               
builddir:
	@mkdir -p build   
             
clean:   clean-subdirs
	@rm -f *.o *.bak *~ *.elf *.elf2flt *.grv *.gdb build/*

#dspcode.o: dspcode.h dspshared.h
dspcode.o: dspshared.h
main.o: dspshared.h

#dspcode.h: dspcode/snes9x_dsp.out
#	lua bin2c.lua $< > $@

include ../../default.rule
