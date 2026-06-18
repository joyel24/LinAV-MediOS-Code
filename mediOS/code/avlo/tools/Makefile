OBJS = imgCreate.o parse_cfg.o
TARGET = imgCreate

GCCFLAGS = -I./ -ggdb3

GCC = gcc

all:  $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $(OBJS) -o $(TARGET)  -lm

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~  $(TARGET)
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
