include Makefile.conf

ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)


LDFLAGS = -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lgcc
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

TARGET = gravity

SUBDIRS = kernel gui apps
#gui
LIBS = kernel/build/*.o gui/build/*.o
#gui/gui.a

all: $(TARGET)

force: clean all        

$(TARGET) : subdirs
	@echo '-----   building kernel'
	$(LD) -N -Tgravity.lds -o $(TARGET).elf $(LIBS) $(LDFLAGS) -Map gravity.map
	@$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
#@$(RM) -f $(TARGET).elf
clean:  clean-subdirs
	@echo '-----   cleaining MAIN'
	@rm -f *.o *.bak *~ $(TARGET).bin $(TARGET).elf include/*~ include/kernel/*~ include/sys_def/*~  include/gui/*~ *.map


        
        
include default.rule