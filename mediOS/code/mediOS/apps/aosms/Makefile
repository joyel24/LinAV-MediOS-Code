include ../../Makefile.conf

ASFLAGS  += -I../../include -I. -Icpu

GCCFLAGS += -I../../include -I. -Icpu -O3

TARGET = aosms

SUBDIRS = cpu drz80

LIBS = ../../lib/libmedios.a

LDFLAGS =  -nostdlib --emit-relocs -L../../lib -lmedios -T$(TARGET_DEVICE)_aones.lds

BUILD_DIR = build

all: builddir $(LIBS) do_build
	@$(LD) -b binary dspcode/aones_dsp.out -b elf32-littlearm \
	 build/*.o ../../lib/*.o -o $(TARGET).med $(LDFLAGS) -Map aones.map
	@rm -f .elf *.gdb *.elf2flt

stdalone: GCCFLAGS += -DBUILD_STDALONE
stdalone: builddir do_build
        
$(LIBS):
	make -C ../../kernel lib_medios 
               
builddir:
	@mkdir -p build   
             
include ../../default.rule
