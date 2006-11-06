#include "aic23.h"

// port config
static MCBSP_Config aic23_portCfg= {
  0x0000,0x0200, /* SPCR : free running mode */
  0x00A0,0x00A1, /* RCR  : 32 bit receive data length */  
  0x00A0,0x00A0, /* XCR  : 32 bit transmit data length */            
  0x0000,0x3000, /* SRGR 1 & 2 */
  0x0000,0x0000, /* MCR  : single channel */
  // VP : frame transmit polarity was wrong (bit 3 has to be cleared)
  0x000E - 8,        /* PCR  : FSX, FSR active low, external FS/CLK source */
  0x0000,
  0x0000,
  0x0000,
  0x0000
};  


static unsigned short * aic23_sndBuf=NULL;
static unsigned short * aic23_sndBuf2=NULL;

static unsigned short aic23_sndBufLen=0;

static MCBSP_Handle aic23_port=INV;

static DMA_Handle aic23_dma=INV;

static short(*aic23_callback)(void*)=NULL;

void aic23_startDma();

static interrupt void aic23_dmaEnd(void){
	unsigned short * tmpbuf;

	// swap buffers
	tmpbuf=aic23_sndBuf;
	aic23_sndBuf=aic23_sndBuf2;
	aic23_sndBuf2=tmpbuf;

	// launch dma on the other buffer
	aic23_startDma();
	
	// call callback to get data
	if(aic23_callback!=NULL) aic23_callback(aic23_sndBuf2);
}

MCBSP_Handle aic23_openPort(){
	aic23_port = MCBSP_open(MCBSP_PORT0, MCBSP_OPEN_RESET);
	MCBSP_config(aic23_port,&aic23_portCfg);
	MCBSP_start(aic23_port,MCBSP_XMIT_START | MCBSP_RCV_START,0);

	return aic23_port;	
}

void aic23_setupDma(unsigned short len,short(*callback)(void*)){
	
	Uint16 dmaEventID=0;

	aic23_callback=callback;

	// open dma channel
	DMA_close(aic23_dma);
 	aic23_dma = DMA_open(DMA_CHA3, DMA_OPEN_RESET);    
    dmaEventID = DMA_getEventId(aic23_dma);      
    DMA_FSET(DMPREC,INTOSEL,DMA_DMPREC_INTOSEL_CH2_CH3);

 	// setup dma irq
    IRQ_globalDisable();
    IRQ_plug(dmaEventID,&aic23_dmaEnd);
	IRQ_clear(dmaEventID);
    IRQ_enable(dmaEventID);
    IRQ_globalEnable();
    
	//alloc both buffers
	if(aic23_sndBuf!=NULL) free(aic23_sndBuf);
	if(aic23_sndBuf2!=NULL) free(aic23_sndBuf2);
        aic23_sndBuf=malloc(len);
	aic23_sndBuf2=malloc(len);
	memset(aic23_sndBuf,0,len);
	memset(aic23_sndBuf2,0,len);

	aic23_sndBufLen=len;
}

void aic23_startDma(){

    unsigned short buffer;

	buffer=(unsigned short)aic23_sndBuf;
	    
    /* Write configuration structure values to DMA control regs */ 
    DMA_configArgs(
	   aic23_dma,    
	   1 ,                                  /* Priority */
	   DMA_DMMCR_RMK(
			 DMA_DMMCR_AUTOINIT_OFF,
			 DMA_DMMCR_DINM_ON,
			 DMA_DMMCR_IMOD_FULL_ONLY,
			 0, /* not ABU */
			 DMA_DMMCR_SLAXS_OFF,
			 DMA_DMMCR_SIND_POSTINC,
			 DMA_DMMCR_DMS_DATA,
			 DMA_DMMCR_DLAXS_OFF,
			 DMA_DMMCR_DIND_NOMOD,
			 DMA_DMMCR_DMD_DATA
			 ),                             /* DMMCR */
	   DMA_DMSFC_RMK(
			 DMA_DMSFC_DSYN_XEVT0,
			 DMA_DMSFC_DBLW_ON,
			 DMA_DMSFC_FRAMECNT_OF(0)
			 ),                             /* DMSFC */ 
	   (DMA_AdrPtr)(buffer+2),              /* DMSRC */
	   (DMA_AdrPtr)MCBSP_ADDR(DXR20),       /* DMDST */
	   (Uint16)(aic23_sndBufLen-2)/2-1      /* DMCTR = buffsize */
	);

    while(!MCBSP_xrdy(aic23_port));
    MCBSP_write32(aic23_port,*(long *)buffer);

    /* Start DMA transfer */
    DMA_start(aic23_dma);
}

