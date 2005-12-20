/*
*   include/ata.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __ATA_H
#define __ATA_H

#include <kernel/hardware.h>
//#include <kernel/disk.h>
#include <kernel/hardware.h>
#include <kernel/target/arch/arch_def.h>

#define IDE_CMD_READ_SECTORS              0x20
#define IDE_CMD_WRITE_SECTORS             0x30
#define IDE_CMD_IDENTIFY                  0xec
#define IDE_CMD_SLEEP                     0xe0

#define IDE_STATUS_BSY                    0x80
#define IDE_STATUS_RDY                    0x40
#define IDE_STATUS_DF                     0x20
#define IDE_STATUS_DRQ                    0x08
#define IDE_STATUS_ERR                    0x01

#define IDE_SEL_LBA                       0x40

#define ATA_DO_READ                       0
#define ATA_DO_WRITE                      1
#define ATA_DO_IDENT                      2
#define ATA_SLEEP                         3

#define ATA_WITH_DMA                             1
#define ATA_NO_DMA                               0

#define WAIT_XFER_TIMEOUT                    0x800000
#define WAIT_READY_TIMEOUT                   0x800000

#define SECTOR_SIZE      512

typedef struct __ATA_CMD {
    int drive;
    int use_dma;
    int xfer_dir;
    void * data;
    int count;
    unsigned int lba;
    struct __ATA_CMD * nxt;
} ata_cmd_s;

//int  ata_RW_Sector     (unsigned int lba,int count,void * buffer,int direction);
int  ata_process_cmd   (ata_cmd_s * ata_cmd);
void ata_RW_Data       (void * buffer,int count,int direction,int use_dma);

int  ata_identify      (int drive,char * buffer);

int  ata_sleep         (void);

int  ata_waitForXfer   (void);
int  ata_waitForReady  (void);
int  ata_status        (void);

void ata_powerUp_HD    (void);
void ata_powerDown_HD  (void);
void ata_select_HD     (void);
void ata_select_CF     (void);

void ata_stop_HD       (void);

void init_ata          (void);
void ide_intr_action   (int irq);

void arch_ata_reset_HD(void);
void arch_ata_powerUp_HD(void);
void arch_ata_powerDown_HD(void);
void arch_ata_select_HD(void);
void arch_ata_select_CF(void);
void arch_init_ata(void);
void arch_ide_intr_action(int irq);

#endif
