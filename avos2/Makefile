SUBDIRS = lib include examples

all: subdirs

subdirs: $(patsubst %, _dir_%, $(SUBDIRS))

$(patsubst %, _dir_%, $(SUBDIRS)) : dummy
	@$(MAKE) -C $(patsubst _dir_%, %, $@)

clean: subdirs_clean
	@echo "**** Cleaning..."
	rm -f *~ *.bak
	
subdirs_clean: $(patsubst %, _dirclean_%, $(SUBDIRS))

$(patsubst %, _dirclean_%, $(SUBDIRS)) : dummy
	@$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

depend: subdirs_depend

subdirs_depend: $(patsubst %, _dirdepend_%, $(SUBDIRS))

$(patsubst %, _dirdepend_%, $(SUBDIRS)) : dummy
	@$(MAKE) -C $(patsubst _dirdepend_%, %, $@) depend

spotless: subdirs_spotless
	
subdirs_spotless: $(patsubst %, _dirspotless_%, $(SUBDIRS))

$(patsubst %, _dirspotless_%, $(SUBDIRS)) : dummy
	@$(MAKE) -C $(patsubst _dirspotless_%, %, $@) spotless

.PHONY: dummy subdirs subdirs_clean subdirs_depend subdirs_spotless
