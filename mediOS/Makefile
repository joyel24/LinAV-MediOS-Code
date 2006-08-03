include Makefile.conf

ASFLAGS  += -I../include -Iinclude
GCCFLAGS += -I../include -Iinclude

#LDFLAGS =  -L/usr/local/cross-tools/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lfloat -lgcc
LDFLAGS =  -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lfloat -lm -lgcc
#LDFLAGS =  -L/usr/local/toolchain_arm_gcc402/lib/ -L/usr/local/toolchain_arm_gcc402/lib/gcc/arm-elf/4.0.2/ -lfloat -lgcc
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

SUBDIRS = kernel
KERNEL_OBJS = kernel/build/*.o kernel/target/arch_$(TARGET_DEVICE)/build/*.o

all: lib_medios medios

force: clean all

mk_api_file:
	@make -C tools/api_files/ --no-print-directory

incdir:
	@make -C include/kernel/target --no-print-directory incdir

lib_medios: incdir mk_api_file
	@echo '-----   building libMedios'
	@make -C kernel --no-print-directory lib_medios
	@echo '-----   building libMedios done'

medios: base_clean medios.bin medios.wav medios.avi

medios.bin: incdir mk_api_file subdirs
	@echo '-----   building kernel'
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o medios.elf $(KERNEL_OBJS) $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary medios.elf medios.bin

aoboy: base_clean aoboy.bin aoboy.wav aoboy.avi

aoboy.bin: incdir mk_api_file
	@make -C kernel/ --no-print-directory kernel_lib
	@make -C apps/avboy --no-print-directory stdalone
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o aoboy.elf $(KERNEL_OBJS) apps/avboy/build/*.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary aoboy.elf aoboy.bin

doom: base_clean doom.bin doom.wav doom.avi

doom.bin: incdir mk_api_file
	@make -C kernel/ --no-print-directory kernel_lib
	@make -C apps/doom --no-print-directory stdalone
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o doom.elf $(KERNEL_OBJS) apps/doom/build/*.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary doom.elf doom.bin

snes9x: snes9x.bin snes9x.wav snes9x.avi

snes9x.bin: incdir mk_api_file
	@make -C kernel/ --no-print-directory kernel_lib
	@make -C apps/snes9x --no-print-directory stdalone
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o snes9x.elf \
	-b binary apps/snes9x/dspcode/snes9x_dsp.out -b elf32-littlearm \
	$(KERNEL_OBJS) apps/snes9x/build/*.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary snes9x.elf snes9x.bin

aones: aones.bin aones.wav aones.avi

aones.bin: incdir mk_api_file
	@make -C kernel/ --no-print-directory kernel_lib
	@make -C apps/aones --no-print-directory stdalone
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o aones.elf \
	-b binary apps/aones/dspcode/aones_dsp.out -b elf32-littlearm \
	$(KERNEL_OBJS) apps/aones/build/*.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary aones.elf aones.bin

uidemo: uidemo.bin uidemo.wav uidemo.avi

uidemo.bin: incdir mk_api_file apps/uidemo.o
	@make -C kernel/ --no-print-directory kernel_lib
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o uidemo.elf $(KERNEL_OBJS) apps/uidemo.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary uidemo.elf uidemo.bin

base_clean:
	@rm -f *.wav *.map *.elf *.bin *.avi

clean:  clean-subdirs base_clean clean_aoboy clean_doom clean_snes9x clean_aones clean_uidemo
	@echo '-----   cleaning MAIN'
	@rm -f *.o *.bak *~
	@rm -f include/*~ include/kernel/*~ include/sys_def/*~  include/gui/*~ include/kernel/target/*~
	@rm -f lib/*

clean_aoboy:
	@make -C apps/avboy --no-print-directory clean

clean_doom:
	@make -C apps/doom --no-print-directory clean

clean_snes9x:
	@make -C apps/snes9x --no-print-directory clean

clean_aones:
	@make -C apps/aones --no-print-directory clean

clean_uidemo:
	@rm -f apps/uidemo.o


include default.rule
