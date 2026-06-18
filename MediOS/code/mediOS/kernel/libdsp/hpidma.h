#ifndef __HPIDMA_H
#define __HPIDMA_H

#include "libdsp.h"

#define HPI_DIR_SD2DSP 0
#define HPI_DIR_DSP2SD 1

extern void hpiDma_start(unsigned long sdAddr,void * dspAddr,unsigned short size, short direction);
extern short hpiDma_pending();

#endif /*__HPIDMA_H*/
