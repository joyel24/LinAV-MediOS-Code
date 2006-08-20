include ../../Makefile.conf

ASFLAGS  += -I../../include -I. -Icpu

GCCFLAGS += -I../../include -I. -O3 \
	-fno-common # doom requires all variables to be zero initialized, this does it by placing them into .bss

#
# !!!!!!!!!!!!!!!!!!!!!!!!!!!
# oxy:
#-fno-common => everything is in bss and I suspect this
# makes it impossible to have doom as med
# when doing med we get strange error from ld :
#arm-linux-uclibc-ld: Warning: alignment 4 of symbol `rtc_setTime' in build/am_map.o is smaller than 32 in
#../../lib/libmedios.a(api.o)

TARGET = doom

LIBS = ../../lib/libmedios.a

LDFLAGS =  -nostdlib --emit-relocs -L../../lib -lmedios -T$(TARGET_DEVICE)_doom.lds

BUILD_DIR = build

all: builddir $(LIBS) do_build
	@$(LD) build/*.o ../../lib/*.o $(LDFLAGS) -o $(TARGET).med -Map doom.map
	@rm -f .elf *.gdb *.elf2flt

stdalone: GCCFLAGS += -DBUILD_STDALONE
stdalone: builddir do_build
        
$(LIBS):
	make -C ../../kernel lib_medios 
               
builddir:
	@mkdir -p build   

include ../../default.rule
