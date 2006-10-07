include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(BUILD_TYPE)","SIMPLE_LOADER")
SRC=fat_lowLevel.c vfs.c vfs_fsManip.c disk.c vfs_dops.c vfs_node.c fat.c vfs_fops.c vfs_pathname.c bin_load.c
else
SRC=bflat.c fat_lowLevel.c vfs.c vfs_fsManip.c disk.c file_type.c vfs_dops.c vfs_node.c fat.c med.c vfs_fops.c vfs_pathname.c bin_load.c
endif


all: do_build

include $(MEDIOS_SRC_PATH)/default.rule