include ../../Makefile.conf

ASFLAGS  += -I../../include -I. -In6502 -Imapper

GCCFLAGS += -I../../include -I. -In6502 -Imapper -O3

TARGET = aones

SUBDIRS = n6502 mapper

LIBS = ../../lib/libmedios.a

LDFLAGS =  -nostdlib --emit-relocs -L../../lib -lmedios -lfloat -T$(TARGET_DEVICE)_aones.lds

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
