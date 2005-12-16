ARCH_NAME=AV3XX

all:
	@$(MAKE) -C include/target/ incdir_$(ARCH_NAME)
	@$(MAKE) -C lib-src/target/ incdir_$(ARCH_NAME)
	@$(MAKE) -C loader/ all
        
clean:
	@$(MAKE) -C loader/ clean
	@rm -f cfg/*~
	@rm -f include/*~
	@rm -f *~