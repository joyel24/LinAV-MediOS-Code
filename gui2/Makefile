include Makefile.conf

all:
ifeq ($(HAVE_JPEG),y)
	$(MAKE) -C jpeg-6b/ all
endif
	$(MAKE) -C graphics/ all
	$(MAKE) -C apps/ all


clean:
	$(MAKE) -C jpeg-6b/ clean
	$(MAKE) -C graphics/ clean
	$(MAKE) -C apps/ clean
	@rm -f *~
