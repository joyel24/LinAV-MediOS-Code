include Makefile.conf

ASFLAGS  += -I../include
GCCFLAGS += -I../include

LDFLAGS =  -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lgcc
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

SUBDIRS = kernel
KERNEL_OBJS = kernel/build/*.o kernel/target/arch_$(TARGET_DEVICE)/*.o

all: medios lib

force: clean all        

mk_api_file:
	@make -C tools/api_files/ --no-print-directory
incdir:
	@make -C include/kernel/target --no-print-directory incdir
        
lib:
	@echo '-----   building libMedios'
	@make -C kernel --no-print-directory lib_medios
	@echo '-----   building libMedios done'

medios: incdir mk_api_file subdirs
	@echo '-----   building kernel'
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o medios.elf $(KERNEL_OBJS) $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary medios.elf medios.bin

aoboy: 
	@make -C kernel/ --no-print-directory kernel_lib
	@make -C apps/avboy --no-print-directory stdalone
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o aoboy.elf $(KERNEL_OBJS) apps/avboy/build/*.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary aoboy.elf aoboy.bin

doom: 
	@make -C kernel/ --no-print-directory kernel_lib
	@make -C apps/doom --no-print-directory stdalone
	@$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o doom.elf $(KERNEL_OBJS) apps/doom/build/*.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary doom.elf doom.bin
                
clean:  clean-subdirs clean_aoboy clean_doom
	@echo '-----   cleaning MAIN'
	@rm -f *.o *.bak *~ *.bin *.elf *.map
	@rm -f include/*~ include/kernel/*~ include/sys_def/*~  include/gui/*~ lib/* include/kernel/target/*~

clean_aoboy:
	@make -C apps/avboy --no-print-directory clean
        
clean_doom:
	@make -C apps/doom --no-print-directory clean        
include default.rule