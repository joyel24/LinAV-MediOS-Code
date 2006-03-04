// !!!!!! penser a remettre -o2 !!!!!!!!!

// VP petites remarques :
//
// sizeof(int) = 1 donc pareil que char et short : un mot de 16 bits
// sizeof(long) = 2 donc 32 bits
//

#include <stdio.h>
#include <string.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_pwr.h>

#include "../dspshared.h"

dsp_com_t dsp_com_buffer;
dsp_com_t * dsp_com;

static MCBSP_Config DataPortCfg= {
  0x0000,0x0200, /* SPCR : free running mode */
  0x00A0,0x0001, /* RCR  : 32 bit receive data length */  
  0x00A0,0x0000, /* XCR  : 32 bit transmit data length */            
  0x0000,0x3000,  
  0x0000,0x0000, /* MCR  : single channel */
  // VP : frame transmit polarity was wrong (bit 3 has to be cleared)
  0x000E - 8,        /* PCR  : FSX, FSR active low, external FS/CLK source */
  0x0000,
  0x0000,
  0x0000,
  0x0000
};     

//Event id's
Uint16 xmtEventID;

static MCBSP_Handle dataPort;

extern void VECSTART(void);

interrupt void writeISR(void);

MCBSP_Handle openAIC23DataPort();

void main(){
  int gie;
#define INC (0x10000*FRQ/32000) 
  //#define INC (0x10000*FRQ/44100) 
  
  *DSP_COM=0;

  CSL_init();
  
  dataPort = MCBSP_open(MCBSP_PORT0, MCBSP_OPEN_RESET);
  
  if (dataPort==INV){
    return;
  }

  dsp_com = &dsp_com_buffer;
  *DSP_COM = (unsigned short) dsp_com;

  gie=IRQ_globalDisable();
  xmtEventID = MCBSP_getXmtEventId(dataPort);
  IRQ_setVecs((Uint32)(&VECSTART));
  IRQ_clear(xmtEventID);
  IRQ_plug(xmtEventID, &writeISR);
  IRQ_enable(xmtEventID);

  MCBSP_config(dataPort,&DataPortCfg);
  
  MCBSP_start(dataPort,MCBSP_XMIT_START,0);

  IRQ_globalEnable();

  while(!dsp_com->stop) {
    PWR_powerDown(PWR_CPU_PER_DOWN, PWR_WAKEUP_MI);
  }

  *DSP_COM = 0;
}

static short l1 = 0, l2 = 0, r1 = 0, r2 = 0;
static long pos = 0;
interrupt void writeISR(void)
{
  unsigned long l, r;
  long ipos = 0x10000L - pos;
    
  IRQ_disable(xmtEventID); // plus besoin de faire ca ...

  l = (l1*ipos + l2*pos);
  r = (r1*ipos + r2*pos);
  r = (l&0xffff0000) | (r>>16);
  MCBSP_write32(dataPort, r);
    
  if (dsp_com->readPos != dsp_com->writePos) {
    pos += INC;
    while (pos >= 0x10000) {
      uint16_t rpos = dsp_com->readPos;
      l1 = l2;
      r1 = r2;
      l2 = dsp_com->buffer[rpos];
      r2 = dsp_com->buffer[rpos+1];
      pos -= 0x10000;
      dsp_com->readPos = (rpos + 2) & (DSP_BUFFER_SIZE-1);
    }
  }

  IRQ_enable(xmtEventID);
}

MCBSP_Handle openAIC23DataPort(){
  MCBSP_Handle port;
  
  port = MCBSP_open(MCBSP_PORT0, MCBSP_OPEN_RESET);
  if (port==INV){
    return INV;
  }
  
  MCBSP_config(port,&DataPortCfg);
  
  MCBSP_start(port,MCBSP_XMIT_START,0);
  
  return port;
}

