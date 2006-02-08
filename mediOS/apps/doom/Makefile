include ../../Makefile.conf

ASFLAGS  += -I../../include -I. -Icpu

GCCFLAGS += -I../../include -I. -O3


TARGET = doom

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

LIBS = ../../lib/libmedios.a

LDFLAGS =  -nostdlib --emit-relocs -L../../lib -lmedios -T$(TARGET_DEVICE)_doom.lds


all: builddir $(LIBS) $(OBJS)
	@cp -f $(OBJS) build
	@$(LD) build/*.o ../../lib/*.o -o $(TARGET).med $(LDFLAGS) -Map doom.map
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

include ../../default.rule
