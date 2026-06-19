ARFLAGS = ruv
AR = ar

OBJCOPY = objcopy
OBJCOPYFLAGS = -O binary

TARGET = arm-elf-

EXE = test
EXEFLAGS = -mthumb -mcallee-super-interworking

LIB = lib/libavos.a

AS = as

ASSRC := $(wildcard *.S)
ASOBJS := $(ASSRC:%.S=%.o)

CDIR := src
CSRC := $(wildcard $(CDIR)/*.c)
COBJS := $(CSRC:%.c=%.o)

all: $(LIB)

$(LIB): $(ASOBJS)
	@if test -a hardware.S; then \
		$(MAKE) -C $(CDIR); \
		$(TARGET)$(AR) $(ARFLAGS) $@ $(ASOBJS) $(COBJS) ; \
	else \
		echo "You must link hardware.S to jbmm.S or av300.S first"; \
	fi

%.o: %.S
	$(TARGET)$(AS) -o $@ common.S hardware.S $<


.PHONY: clean
clean:
	-rm -f  $(ASOBJS) $(COBJS) $(LIB)
