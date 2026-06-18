include Makefile.conf

all:
	@$(MAKE) -C include/target/ incdir_$(TARGET_DEVICE)
	@$(MAKE) -C lib-src/target/ incdir_$(TARGET_DEVICE)
	@$(MAKE) -C loader/target/ incdir_$(TARGET_DEVICE)
	@$(MAKE) -C loader/ all
        
clean:
	@$(MAKE) -C loader/ clean
	@rm -f cfg/*~
	@rm -f include/*~
	@rm -f *~