/*
*   include/kernel/dma.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#ifndef __DMA_H
#define __DMA_H

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/target/arch/dma.h>

#define DMA_SET_SRC(addr)      outw(((addr)>>16)&0xFFFF,DMA_SRC_HI);outw((addr)&0xFFFF,DMA_SRC_LO);
#define DMA_SET_DEST(addr)     outw(((addr)>>16)&0xFFFF,DMA_DEST_HI);outw((addr)&0xFFFF,DMA_DEST_LO);
#define DMA_SET_SIZE(size)     outw((size)&0xFFFF,DMA_SIZE);
/* needed for JBMM: */
#ifndef DMA_SET_DEV
#define DMA_SET_DEV(src,dest)  outw((((src)<<4)&0x00F0) | ((dest)&0x000F),DMA_DEV_SEL);
#endif
#define DMA_START_TRANSFER     outw(inw(DMA_START) | 0x1,DMA_START);
#define DMA_RUNNING            ((inw(DMA_STATE)&0x1)==0x1)
#define DMA_ENDIAN_CONV(type)  outw((inw(DMA_START)&0xFCFF) | ((type)<<16),DMA_START);

#endif
