include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

OBJS = ctype.o memchr.o memcpy.o memset.o memzero.o qsort.o strchr.o random.o string.o strrchr.o vsprintf.o
OBJS2 = api.o 

SUBDIRS = apps_init

all: clean subdirs $(OBJS)
	@cp -f $(OBJS) ../build

lib: GCCFLAGS += -DBUILD_LIB
lib:  clean subdirs   $(OBJS) $(OBJS2)
        
kernel_lib: all
                
force: clean all        

clean: clean-subdirs
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule