include ../Makefile.conf

ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)


SUBDIRS = init core lib driver gfx fs

TARGET = kernel.a

all: builddir subdirs $(TARGET) lib_gravity

force: clean all        

$(TARGET) :
	@echo '-----   building kernel'
#	@$(AR) -rc $(TARGET) build/*.o
#	@$(RANLIB) $(TARGET)
clean:  clean-subdirs
	@echo '-----   cleaining kernel'
	@rm -f *.o *.bak *~ build/*.o $(TARGET)

builddir:
	@mkdir -p build
        
lib_gravity:
	@make -C lib --no-print-directory -s all
	@$(AR) -rc libgravity.a lib/*.o
	@$(RANLIB) libgravity.a
	@mv libgravity.a ../
        
include ../default.rule