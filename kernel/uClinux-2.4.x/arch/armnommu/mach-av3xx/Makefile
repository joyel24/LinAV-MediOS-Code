#
# Makefile for the linux kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).

USE_STANDARD_AS_RULE := true

O_TARGET		:= av3xx.o

# Object file lists.

obj-y			:= $(patsubst %.c, %.o, $(wildcard *.c))
obj-m			:=
obj-n			:=
obj-			:=

obj-y			:= $(obj-y) boot.o

#obj-y			:= $(obj-y) av3xx_graphics.o av3xx_osd.o
#obj-y			:= $(obj-y) fonts/4x6.o fonts/4x8.o fonts/5x7.o fonts/5x8.o fonts/6x9.o fonts/6x10.o \
#							fonts/6x12.o fonts/6x13.o fonts/7x13.o fonts/7x14.o fonts/8x13.o fonts/cursive_9x15.o \
#							fonts/dagger_9x14.o fonts/inkblot_9x13.o fonts/radon_8x12.o fonts/radonWide_9x12.o\
#							fonts/shadow_12x18.o fonts/shadowBold_12x18.o

#obj-y			:= $(obj-y) av3xx_kbd_tst.o

export-objs		:=

include $(TOPDIR)/Rules.make
