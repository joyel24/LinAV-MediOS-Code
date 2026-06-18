#ifndef __IBDMA_H
#define __IBDMA_H

#include "libdsp.h"

#define IB_DSC25_BUFA_ADDR ((void *)0x8000)
#define IB_DSC25_BUFB_ADDR ((void *)0x9000)

#define IB_DM270_BUFA_ADDR ((void *)0xc000)
#define IB_DM270_BUFB_ADDR ((void *)0xd000)

#define IB_DM320_BUFA_ADDR ((void *)0xc000)
#define IB_DM320_BUFB_ADDR ((void *)0xc000)
#define IB_DM320_BUFC_ADDR ((void *)0xc000)

#define IB_DSC25_BUF_SHIFT 2
#define IB_DM270_BUF_SHIFT 2
#define IB_DM320_BUF_SHIFT 3

#define IB_BUF_A 0
#define IB_BUF_B 1
#define IB_BUF_C 2

#define IB_DIR_IB2SD 0
#define IB_DIR_SD2IB 1

extern void ibDma_start(unsigned long sdAddr,unsigned short sdOffset,
				 unsigned short ibAddr, unsigned short ibOffset,
				 unsigned short x,unsigned short y,
				 short buffer, short direction, short byteToWord);

extern short ibDma_pending();

extern void ibDma_reset();

#endif /*__IBDMA_H*/

