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

mk_api_file:
	make -C tools/api_files/
incdir:
	make -C include/kernel/target incdir
        
lib : incdir mk_api_file
	#make -C apps/avboy stdalone
	make -C apps/ othello.o
	make -C kernel/ kernel_lib
	$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o $(TARGET).elf $(LIBS) -Lapps/avboy -lavboy $(LDFLAGS) -Map medios.map
	#$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o $(TARGET).elf $(LIBS) apps/othello.o  $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
    
    

$(TARGET) : incdir mk_api_file subdirs
	@echo '-----   building kernel'
	$(LD) -N -T$(TARGET_DEVICE)_mediOS.lds -o $(TARGET).elf $(LIBS) $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

clean:  clean-subdirs
	@echo '-----   cleaining MAIN'
	@rm -f *.o *.bak *~ $(TARGET).bin $(TARGET).elf *.map libKmedios.a libKboot.a
	@rm -f include/*~ include/kernel/*~ include/sys_def/*~  include/gui/*~ 


        
        
include default.rule