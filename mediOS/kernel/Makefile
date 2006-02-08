include ../Makefile.conf

ASFLAGS  += -I../include
GCCFLAGS += -I../include

SUBDIRS = init core lib driver gfx fs target

all: builddir subdirs

force: clean all        


clean:  clean-subdirs
	@echo '-----   cleaining kernel'
	@rm -f *.o *.bak *~ build/*.o $(TARGET)

lib_clean:
	@rm -f ../lib/*
        
builddir:
	@mkdir -p build
        
kernel_lib: builddir subdirs_lib 

lib_medios:
	@mkdir -p ../lib
	@make -C lib --no-print-directory -s lib        
	@$(AR) -rc libmedios.a lib/*.o
	@$(RANLIB) libmedios.a
	@mv libmedios.a ../lib
        
include ../default.rule