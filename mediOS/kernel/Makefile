include ../Makefile.conf

ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)


SUBDIRS = init core lib driver gfx fs

TARGET = kernel.a

all: builddir subdirs $(TARGET) lib_medios

force: clean all        

$(TARGET) :
	@echo '-----   building kernel'
#	@$(AR) -rc $(TARGET) build/*.o
#	@$(RANLIB) $(TARGET)
clean:  clean-subdirs
	@echo '-----   cleaining kernel'
	@rm -f *.o *.bak *~ build/*.o $(TARGET) ../libmedios.a

builddir:
	@mkdir -p build
        
kernel_lib: builddir subdirs_lib 

lib_medios:
	@make -C lib --no-print-directory -s lib
	@$(AR) -rc libLmedios.a lib/*.o
	@$(RANLIB) libLmedios.a
	@mv libLmedios.a ../
        
include ../default.rule