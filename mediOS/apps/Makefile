MEDIOS_SRC_PATH_TMP = $(shell pwd)
export MEDIOS_SRC_PATH = $(dir $(MEDIOS_SRC_PATH_TMP))

include $(MEDIOS_SRC_PATH)/Makefile.conf

LIBS = $(MEDIOS_SRC_PATH)/lib/libmedios.a

LDFLAGS = $(MEDIOS_SRC_PATH)/lib/*.o -L$(MEDIOS_SRC_PATH)/lib -lmedios --emit-relocs

all: $(LIBS) othello test apps

othello: $(LIBS) othello.o
	@$(LD) -o othello.med othello.o $(LDFLAGS)

test: $(LIBS) test.o
	@$(LD) -o test.med test.o $(LDFLAGS)
  
uidemo: $(LIBS) uidemo.o
	$(LD) -o uidemo.med uidemo.o $(LDFLAGS)
      
$(LIBS):
	@make -C $(MEDIOS_SRC_PATH) --no-print-directory medios_lib

SUBDIRS = doom avboy
        
apps: subdirs
	@make -C doom --no-print-directory
	@make -C avboy --no-print-directory

include ../default.rule