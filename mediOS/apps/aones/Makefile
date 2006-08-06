include ../../Makefile.conf

ASFLAGS  += -I../../include -I. -In6502 -Imapper

GCCFLAGS += -I../../include -I. -In6502 -Imapper -O3

TARGET = aones

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

SUBDIRS = n6502 mapper

LIBS = ../../lib/libmedios.a

LDFLAGS =  -nostdlib --emit-relocs -L../../lib -lmedios -lfloat -T$(TARGET_DEVICE)_aones.lds


all: subdirs builddir $(LIBS) $(OBJS)
	@cp -f $(OBJS) build
	@$(LD) -b binary dspcode/ao nes_dsp.out -b elf32-littlearm \
	 build/*.o ../../lib/*.o -o $(TARGET).med $(LDFLAGS) -Map aones.map
	@rm -f .elf *.gdb *.elf2flt
        

stdalone: GCCFLAGS += -DBUILD_STDALONE
stdalone: builddir subdirs $(OBJS)
	@cp -f $(OBJS) build
        
$(LIBS):
	make -C ../../kernel lib_medios 
               
builddir:
	@mkdir -p build   
             
include ../../default.rule
