/* 
*   include/dma.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#ifndef __DMA_H
#define __DMA_H

#include <kernel/io.h>
#include <kernel/hardware.h>

#define DMA_SRC_HI         (DMA_BASE+0x00)
#define DMA_SRC_LO         (DMA_BASE+0x02)
#define DMA_DEST_HI        (DMA_BASE+0x04)
#define DMA_DEST_LO        (DMA_BASE+0x06)
#define DMA_SIZE           (DMA_BASE+0x08)
#define DMA_DEV_SEL        (DMA_BASE+0x0A)
#define DMA_START          (DMA_BASE+0x0C)
#define DMA_STATE          (DMA_BASE+0x0C)

#define DMA_SDRAM          0x5
#define DMA_ATA            0x1

#define dma_set_src(addr)      outw((addr>>16)&0xFFFF,DMA_SRC_HI);outw(addr&0xFFFF,DMA_SRC_LO);
#define dma_set_dest(addr)     outw((addr>>16)&0xFFFF,DMA_DEST_HI);outw(addr&0xFFFF,DMA_DEST_LO);
#define dma_set_size(size)     outw(size&0xFFFF,DMA_SIZE);
#define dma_set_dev(src,dest)  outw(((src<<4)&0x00F0) | (dest&0x000F),DMA_DEV_SEL);
#define dma_start              outw(inw(DMA_START) | 0x1,DMA_START);
#define dma_running            ((inw(DMA_STATE)&0x1)==0x1)
#define dma_endian_conv(type)  outw((inw(DMA_START)&0xFCFF) | (type<<16),DMA_START);

#endif
