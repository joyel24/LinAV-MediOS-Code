include ../Makefile.conf

ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)


SUBDIRS = 

TARGET = gui.a

all: builddir subdirs $(TARGET) 

force: clean all        

$(TARGET) : $(OBJS)
	@cp -f $(OBJS) build/
	@echo '-----   building gui'
#	@$(AR) -rc $(TARGET) build/*.o
#	@$(RANLIB) $(TARGET)
clean:  clean-subdirs
	@echo '-----   cleaning gui'
	@rm -f *.o *.bak *~ build/*.o $(TARGET)

builddir:
	@mkdir -p build
        
include ../default.rule