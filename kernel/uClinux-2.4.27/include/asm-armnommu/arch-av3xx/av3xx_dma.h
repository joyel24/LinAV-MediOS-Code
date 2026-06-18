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

#ifndef __ASM_ARCH_AV3XX_DMA_H
#define __ASM_ARCH_AV3XX_DMA_H

#define AV3XX_DMA_SRC_HI         (AV3XX_DMA_BASE+0x00)
#define AV3XX_DMA_SRC_LO         (AV3XX_DMA_BASE+0x02)
#define AV3XX_DMA_DEST_HI        (AV3XX_DMA_BASE+0x04)
#define AV3XX_DMA_DEST_LO        (AV3XX_DMA_BASE+0x06)
#define AV3XX_DMA_SIZE           (AV3XX_DMA_BASE+0x08)
#define AV3XX_DMA_DEV_SEL        (AV3XX_DMA_BASE+0x0A)
#define AV3XX_DMA_START          (AV3XX_DMA_BASE+0x0B)
#define AV3XX_DMA_STATE          (AV3XX_DMA_BASE+0x0B)

#define AV3XX_DMA_SDRAM          0x5
#define AV3XX_DMA_ATA            0x1

#define AV_DMA_READ              0
#define AV_DMA_WRITE             1

/*30a38   DMA source address HI
30a3a   DMA source address LO
30a3c   DMA destination address HI
30a3e   DMA destination address LO
30a40   DMA data size (In 32bit words)
30a42   DMA device select. Low nibble = destination, High nibble = source.
        5=SDRAM, 1=ATA
30a44   DMA start. Bit 0 - Write: Start DMA xfer. Read: Is DMA running/finished?
        0x100 = endian_2301
        0x200 = endian_0123
*/

int av_ata_RW_DMA_Sector(unsigned int lba,int size,void * buffer,int direction);

#endif
