/*
*   include/kernel/ata.h
*
*   MediOS project
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
#include <kernel/irq.h>

extern int ata_stopping;

#define ATA_PWR_OFF_TASK {     \
    int __status;              \
    if(ata_stopping)           \
    {                          \
        ata_stopping++;        \
        __status=ata_status();   \
        if(((__status & IDE_STATUS_BSY)==0 && (__status & IDE_STATUS_RDY)!=0) || ata_stopping > 1000) \
        {                      \
            if(ata_stopping > 1000) printk("ata_stopping timout\n"); \
            ata_stopHDEnd();   \
            ata_stopping = 0;  \
        }                      \
    }                          \
}

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

#define ATA_WITH_DMA                             1
#define ATA_NO_DMA                               0

#define WAIT_XFER_TIMEOUT                    0x800000
#define WAIT_READY_TIMEOUT                   0x800000

#define ATA_FORCE_STOP                   0  /* power off is not split in 2 parts + timer */
#define ATA_DELAY_STOP                   1  /* power off wait loop is executed in background */

#define SECTOR_SIZE      512

int  ata_rwData           (int drive,unsigned int lba,void * data,int count,int xfer_dir,int use_dma);

int  ata_waitForXfer      (void);
int  ata_waitForReady     (void);
int  ata_status           (void);

void ata_powerUpHD        (void);
void ata_powerDownHD      (void);
void ata_stopHD           (int mode);  /* mode should be: ATA_FORCE_STOP or ATA_DELAY_STOP */
void ata_stopHDEnd        (void);
int  ata_sleep            (void);
void ata_sofReset         (void);

void ata_selectHD         (void);
void ata_selectCF         (void);

void ata_init             (void);

void arch_ata_resetHD     (void);
void arch_ata_powerUpHD   (void);
void arch_ata_powerDownHD (void);
void arch_ata_selectHD    (void);
void arch_ata_selectCF    (void);
void arch_ata_init        (void);
void arch_ide_intAction   (int irq,struct pt_regs * regs);

void ide_intAction        (int irq,struct pt_regs * regs);

#endif
