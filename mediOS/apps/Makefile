MEDIOS_SRC_PATH_TMP = $(shell pwd)
export MEDIOS_SRC_PATH = $(dir $(MEDIOS_SRC_PATH_TMP))

include $(MEDIOS_SRC_PATH)/Makefile.conf

MED_LIST = $(patsubst %.c,%.med,$(wildcard *.c))

LIBS = $(MEDIOS_SRC_PATH)/lib/libmedios.a

LDFLAGS = $(MEDIOS_SRC_PATH)/lib/*.o -L$(MEDIOS_SRC_PATH)/lib -lmedios --emit-relocs

$(LIBS):
	@make -C $(MEDIOS_SRC_PATH) --no-print-directory medios_lib

all_med:
	@for p in $(MED_LIST); do \
		make $$p; \
	done
        
%.med: %.o $(LIBS)
	@$(LD) -o $@ $< $(LDFLAGS)

include ../default.rule