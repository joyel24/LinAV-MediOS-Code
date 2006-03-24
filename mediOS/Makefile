include Makefile.conf

ASFLAGS  += -I../include
GCCFLAGS += -I../include

#LDFLAGS =  -L/usr/local/cross-tools/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lfloat -lgcc
LDFLAGS =  -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lfloat -lgcc
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

SUBDIRS = kernel
KERNEL_OBJS = kernel/build/*.o kernel/target/arch_$(TARGET_DEVICE)/*.o

all: lib_medios medios

force: clean all

mk_api_file:
	@make -C tools/api_files/ --no-print-directory

tools/bin2wav/bin2wav:        
	@make -C tools/bin2wav/ --no-print-directory

incdir:
	@make -C include/kernel/target --no-print-directory incdir

lib_medios: incdir mk_api_file
	@echo '-----   building libMedios'
	@make -C kernel --no-print-directory lib_medios
	@echo '-----   building libMedios done'

medios: base_clean medios.bin medios.wav

medios.bin: incdir mk_api_file subdirs
	@echo '-----   building kernel'
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o medios.elf $(KERNEL_OBJS) $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary medios.elf medios.bin

aoboy: base_clean aoboy.bin aoboy.wav

aoboy.bin: incdir mk_api_file
	@make -C kernel/ --no-print-directory kernel_lib
	@make -C apps/avboy --no-print-directory stdalone
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o aoboy.elf $(KERNEL_OBJS) apps/avboy/build/*.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary aoboy.elf aoboy.bin

doom: base_clean doom.bin doom.wav

doom.bin: incdir mk_api_file
	@make -C kernel/ --no-print-directory kernel_lib
	@make -C apps/doom --no-print-directory stdalone
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o doom.elf $(KERNEL_OBJS) apps/doom/build/*.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary doom.elf doom.bin

snes9x: snes9x.bin snes9x.wav

snes9x.bin: incdir mk_api_file
	@make -C kernel/ --no-print-directory kernel_lib
	@make -C apps/snes9x --no-print-directory stdalone
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o snes9x.elf \
	-b binary apps/snes9x/dspcode/snes9x_dsp.out -b elf32-littlearm \
	$(KERNEL_OBJS) apps/snes9x/build/*.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary snes9x.elf snes9x.bin


base_clean:
	@rm -f *.wav *.map *.elf *.bin

clean:  clean-subdirs base_clean clean_aoboy clean_doom 
	@echo '-----   cleaning MAIN'
	@rm -f *.o *.bak *~
	@rm -f include/*~ include/kernel/*~ include/sys_def/*~  include/gui/*~ include/kernel/target/*~
	@rm -f lib/*

clean_aoboy:
	@make -C apps/avboy --no-print-directory clean

clean_doom:
	@make -C apps/doom --no-print-directory clean
include default.rule