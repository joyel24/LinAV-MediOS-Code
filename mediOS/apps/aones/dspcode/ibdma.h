#ifndef __IBDMA_H
#define __IBDMA_H

#define IB_BUFA_ADDR ((void *)0xc000)
#define IB_BUFB_ADDR ((void *)0xd000)

#define IB_BUF_A 0
#define IB_BUF_B 2

#define IB_DIR_IB2SD 0
#define IB_DIR_SD2IB 1

void ibdma_start(unsigned long sdAddr,unsigned short sdOffset,
				 unsigned short ibAddr, unsigned short ibOffset,
				 unsigned short x,unsigned short y,
				 short buffer, short direction, short byteToWord);

short ibdma_pending();

void ibdma_reset();

#endif /*__IBDMA_H*/
