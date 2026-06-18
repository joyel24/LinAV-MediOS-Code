# Lib makefile 
include ../Makefile.conf

ASFLAGS  += -I../include
GCCFLAGS += -I../include

TARGET=libav.a
DEST=../build/lib/


OBJS = ata.o ctype.o disk.o file.o graphics_32.o graphics.o string.o uart.o cpld.o dir.o fat.o i2c.o
OBJS += printk.o vsprintf.o delay.o memchr.o memcpy.o memset.o memzero.o strchr.o strrchr.o

ifeq ("$(TARGET_DEVICE)","AV3XX")
OBJS += osd_DSC25.o graphics_8_DSC25.o tsc2003.o
endif

ifeq ("$(TARGET_DEVICE)","AV1XX")
OBJS += osd_DSC25.o graphics_8_DSC25.o
endif

ifeq ("$(TARGET_DEVICE)","JBMM")
OBJS += osd_DSC21.o graphics_8_DSC21.o
endif

SUBDIRS = fonts/ target/

all: $(TARGET)

$(TARGET): subdirs $(OBJS)
	$(AR) -rc $(TARGET) $(OBJS) fonts/*.o target/arch/*.o
	$(RANLIB) $(TARGET)
	$(MV) $(TARGET) $(DEST)

clean: clean-subdirs 
	@rm -f *.o *.O *.bak *~ $(BUILD_DIR)/lib-src/$(TARGET)
    
include ../default.rule

# End of Makefile
