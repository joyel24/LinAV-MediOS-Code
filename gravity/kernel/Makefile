include Makefile.conf

ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)


LDFLAGS = -L/home/thomas/Av320/arm-tools/inst_uclibc/toolchain/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc 

TARGET = gravity

SUBDIRS = init core lib driver gfx

all: builddir subdirs $(TARGET) 

force: clean all        

$(TARGET) : $(OBJS)
	@echo '-----   building kernel'
#	@cp -f $(OBJS) build/
	$(LD) -N -Tgravity.lds -o $(TARGET).elf *.o build/*.o $(LDFLAGS) -Map gravity.map
	@$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
#@$(RM) -f $(TARGET).elf
clean:  clean-subdirs
	@echo '-----   cleaining MAIN'
	@rm -f *.o *.bak *~ $(TARGET).bin $(TARGET).elf ../include/*~ ../include/kernel/*~ ../include/sys_def/*~ build/*.o *.map

builddir:
	@mkdir -p build
        
include default.rule