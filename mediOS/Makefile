include Makefile.conf

ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)


LDFLAGS =  -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lgcc
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

TARGET = medios

SUBDIRS = kernel
#apps
#gui
LIBS = kernel/build/*.o kernel/target/arch_$(TARGET_DEVICE)/*.o
#gui/gui.a

all: $(TARGET)

force: clean all        

incdir:
	make -C include/kernel/target incdir
        
lib : incdir
#	make -C apps/sms stdalone
	make -C apps/avboy stdalone
        
#	make -C apps/ othello_iram.o
#	make -C apps/ othello_sdram.o
#	make -C apps/ othello.o
	make -C kernel/ kernel_lib
#	$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o $(TARGET).elf $(LIBS) -Lapps/sms -lsms $(LDFLAGS) -Map medios.map
	$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o $(TARGET).elf $(LIBS) -Lapps/avboy -lavboy $(LDFLAGS) -Map medios.map
#	$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o $(TARGET).elf $(LIBS) apps/othello.o  $(LDFLAGS) -Map medios.map
#	$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o $(TARGET).elf $(LIBS) apps/othello_iram.o apps/othello_sdram.o $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
#	@$(AR) -rc libKmedios.a kernel/build/*.o
#	@$(RANLIB) libKmedios.a        
#	@$(AR) -rc libKboot.a kernel/init/boot.o
#	@$(RANLIB) libKboot.a
    
    

$(TARGET) : incdir subdirs
	@echo '-----   building kernel'
	$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o $(TARGET).elf $(LIBS) $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
#@$(RM) -f $(TARGET).elf
clean:  clean-subdirs
	@echo '-----   cleaining MAIN'
	@rm -f *.o *.bak *~ $(TARGET).bin $(TARGET).elf *.map libKmedios.a libKboot.a
	@rm -f include/*~ include/kernel/*~ include/sys_def/*~  include/gui/*~ 


        
        
include default.rule