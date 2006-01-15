# General config and build tools
include ../Makefile.conf

# State the main target to build

TARGET = avlo
BUILD_DIR = ../build



ASFLAGS  += -I../include
GCCFLAGS += -I../include

#LOCAL FLAGS
LDFLAGS  = -L../build/lib -lav -lfloat 
#LDFLAGS += -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lgcc
LDFLAGS +=  -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lgcc

OBJS = bootstart.o avlo.o parse_cfg.o 

ifeq ($(HAVE_IMG),1)
OBJS += img.o
endif

all:$(TARGET)

SUBDIRS = ../lib-src

# Rule to make executable
$(TARGET) : subdirs $(OBJS)
	$(LD) -N -T../$(TARGET_DEVICE)_AvLo.lds -o $(TARGET).elf $(OBJS) $(LDFLAGS)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	@$(RM) -f $(TARGET).elf
	@$(MV) -f $(TARGET).bin $(BUILD_DIR)


    
clean: clean-subdirs
	@echo "**** Cleaning..."
	@rm -f *.o *.O *.bak *~ *.elf $(BUILD_DIR)/*.bin
        
      

include ../default.rule

# END of Makefile