include ../../Makefile.conf

ASFLAGS  += -I../../include
GCCFLAGS += -I../../include

#SRC = $(wildcard *.c)
#SRC2 = $(wildcard *.S)
#OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

OBJS = ctype.o memchr.o memcpy.o memset.o memzero.o qsort.o strchr.o random.o string.o strrchr.o vsprintf.o
OBJS2 = api.o 

all: clean $(OBJS)
	@cp -f $(OBJS) ../build

lib: GCCFLAGS += -DBUILD_LIB
lib:  clean $(OBJS) $(OBJS2)
        
kernel_lib: all
                
force: clean all        

clean:  
	@rm -f *.o *.bak *~ $(TARGET)

include ../../default.rule