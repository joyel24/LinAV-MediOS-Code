include $(MEDIOS_SRC_PATH)/Makefile.conf

SRC = $(FONT_LIST:%=%.c)

GCCFLAGS += -I$(MEDIOS_SRC_PATH)/include/kernel

OPT = $(FONT_LIST:%=-D_std%)

BUILD_DIR = ../

all: do_build font

font:
	@$(GCC) $(GCCFLAGS) -c -o font.o font.c $(OPT)
	@cp -f font.o ../

include $(MEDIOS_SRC_PATH)/default.rule