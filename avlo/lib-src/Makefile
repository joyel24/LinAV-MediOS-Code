# Lib makefile 
include ../Makefile.conf

ASFLAGS  += -I../include
GCCFLAGS += -I../include

TARGET=libav.a
DEST=../build/lib

SRC1 = $(wildcard *.c)
OBJS1 = $(SRC1:%.c=%.o)
SRC2 = $(wildcard *.s)
OBJS2 = $(SRC2:%.s=%.o)
OBJS=$(OBJS1) $(OBJS2)

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
