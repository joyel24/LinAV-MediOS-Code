include Makefile.conf

#LDFLAGS =  -L/usr/local/cross-tools/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lfloat -lgcc
LDFLAGS =  -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lfloat -lm -lgcc
#LDFLAGS =  -L/usr/local/toolchain_arm_gcc402/lib/ -L/usr/local/toolchain_arm_gcc402/lib/gcc/arm-elf/4.0.2/ -lfloat -lgcc
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

SUBDIRS = kernel
KERNEL_OBJS = kernel/build/*.o kernel/target/arch_$(TARGET_DEVICE)/build/*.o

export MEDIOS_SRC_PATH=$(shell pwd)

all: medios

mk_api_file:
	@make -C tools/api_files/ --no-print-directory

incdir:
	@make -C include/kernel/target --no-print-directory incdir

medios: medios.bin medios.wav medios.avi

medios.bin: incdir mk_api_file subdirs
	@echo '-----   Linking medios.bin'
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o medios.elf $(KERNEL_OBJS) $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary medios.elf medios.bin

medios_lib: incdir mk_api_file
	@make -C $(MEDIOS_SRC_PATH)/kernel --no-print-directory -s medios_lib

aoboy: APP_NAME = avboy
aoboy: do_build_app

doom: APP_NAME = doom
doom: do_build_app

uidemo: APP_NAME = uidemo
uidemo: do_build_simple_app

snes9x: LDFLAGS += -b binary apps/snes9x/dspcode/snes9x_dsp.out -b elf32-littlearm
snes9x: APP_NAME = snes9x
snes9x: do_build_app
     
aones: LDFLAGS += -b binary apps/aones/dspcode/aones_dsp.out -b elf32-littlearm
aones: APP_NAME = aones
aones: do_build_app

include default.rule
