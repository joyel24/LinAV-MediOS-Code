#ifndef __AIC23_H
#define __AIC23_H

#include <stdlib.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_dma.h>

#include "libdsp.h"

extern MCBSP_Handle aic23_openPort();
extern void aic23_setupDma(unsigned short len,short(*callback)(void*));
extern void aic23_startDma();

#endif /*__AIC23_H*/
