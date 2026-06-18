/*
* asm/arch/av3xx_ide.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __ASM_ARCH_AV3XX_ATA_IDE_H
#define __ASM_ARCH_AV3XX_ATA_IDE_H

#define AV3XX_IDE_DATA                          (AV3XX_IDE_BASE+0x000)
#define AV3XX_IDE_ERROR                         (AV3XX_IDE_BASE+0x080)
#define AV3XX_IDE_NSECTOR                       (AV3XX_IDE_BASE+0x100)
#define AV3XX_IDE_SECTOR                        (AV3XX_IDE_BASE+0x180)
#define AV3XX_IDE_LCYL                          (AV3XX_IDE_BASE+0x200)
#define AV3XX_IDE_HCYL                          (AV3XX_IDE_BASE+0x280)
#define AV3XX_IDE_SELECT                        (AV3XX_IDE_BASE+0x300)
#define AV3XX_IDE_CONTROL                       (AV3XX_IDE_BASE+0x340)
#define AV3XX_IDE_STATUS                        (AV3XX_IDE_BASE+0x380)
#define AV3XX_IDE_COMMAND                       (AV3XX_IDE_BASE+0x380)

#define AV3XX_IDE_CMD_READ_SECTORS              0x20
#define AV3XX_IDE_CMD_WRITE_SECTORS             0x30
#define AV3XX_IDE_CMD_IDENTIFY                  0xec
#define AV3XX_IDE_CMD_SLEEP                     0xe0

#define AV3XX_IDE_STATUS_BSY                    0x80
#define AV3XX_IDE_STATUS_RDY                    0x40
#define AV3XX_IDE_STATUS_DF                     0x20
#define AV3XX_IDE_STATUS_DRQ                    0x08
#define AV3XX_IDE_STATUS_ERR                    0x01

#define AV3XX_IDE_SEL_LBA                       0x40

#define AV_DO_READ                              0
#define AV_DO_WRITE                             1

#define AV_WAIT_XFER_TIMEOUT                    0x200000
#define AV_WAIT_READY_TIMEOUT                   0x200000

struct av_partInfo {
    int start;
    int size;
    int type;
};

struct av_hd_info{
	char serial[21];
	char firmware[9];
	char model [41];
        unsigned long size;
};

/*int  ataReadSectors(unsigned int lba,int count,void * buffer);
int  ataWriteSectors(unsigned int lba,int count,void * buffer);
int  ataIdentifyDevice(void * buffer);
void ataRead(unsigned int lba,int count);
void ataWrite(unsigned int lba,int count);
void ataIdentify(void);
void ataReadData(void * buffer,int * nbHalfWord);
void ataWriteData(void * buffer,int * nbHalfWord);
int  ataWaitForXfer(void);
int  ataWaitForReady(void);
int  ataStatus(void);*/

int  av_ata_RW_Sector     (unsigned int lba,int count,void * buffer,int direction);
void av_ata_RW_Data       (void * buffer,int count,int direction);

int  av_ata_readMBR       (struct av_partInfo * part_list);
int  av_ata_identify      (struct av_hd_info * hd_info);

int  av_ata_sleep         (void);

int  av_ata_waitForXfer   (void);
int  av_ata_waitForReady  (void);
int  av_ata_status        (void);

void av_ata_powerUp_HD    (void);
void av_ata_powerDown_HD  (void);
void av_ata_select_HD     (void);
void av_ata_select_CF     (void);

void av_ata_stop_HD       (void);


#endif
