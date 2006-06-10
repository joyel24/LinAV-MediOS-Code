include ../../../Makefile.conf

SUBDIRS = core driver gfx init
#fs

all: builddir subdirs 

force: clean all        

clean:  clean-subdirs
	@echo '-----   cleaining kernel'
	@rm -f *.o build/*.o

builddir:
	@mkdir -p build
       
kernel_lib: all
        
include ../../../default.rule