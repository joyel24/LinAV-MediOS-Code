include Makefile.conf

all:
ifeq ($(HAVE_JPEG),y)
	@echo "----- building jpeg lib"
	@$(MAKE) -C jpeg-6b/ --no-print-directory all
endif
	@$(MAKE) -C graphics/ --no-print-directory all
	@$(MAKE) -C apps/ --no-print-directory all


all-clean: clean
	@echo "----- jpeg lib cleaned"
	@$(MAKE) -C jpeg-6b/ --no-print-directory clean
        
clean:
	@$(MAKE) -C graphics/ --no-print-directory clean
	@$(MAKE) -C apps/ --no-print-directory clean
	@rm -f *~
