include ../../Makefile.conf

SUBDIRS = arch_$(TARGET_DEVICE)

all: subdirs 

force: clean all        

clean:  clean-subdirs
	@echo '-----   cleaining kernel'
	@rm -f *.o 

       
kernel_lib: all
        
include ../../default.rule