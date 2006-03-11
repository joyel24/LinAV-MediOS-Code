
all:
	@$(MAKE) -C arch all

incdir_AV3XX:
	@rm -f arch
	@ln -s arch_AV3XX arch

incdir_AV1XX:
	@rm -f arch
	@ln -s arch_AV1XX arch

force: clean all        

clean:
	@$(MAKE) -C arch clean
	@rm -f *.o 
