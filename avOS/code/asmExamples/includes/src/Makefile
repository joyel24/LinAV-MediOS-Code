CFLAGS = -Wall
INCLUDE = -I../include

CC = gcc

TARGET = arm-elf-

CSRC := $(wildcard *.c)
COBJS := $(CSRC:%.c=%.o)
COBJFLAGS = -mthumb


all: $(COBJS)

%.o: %.c
	$(TARGET)$(CC) $(CFLAGS) $(COBJFLAGS) -c -I./ $(INCLUDE) $<

clean:
	-rm -f $(COBJS)
