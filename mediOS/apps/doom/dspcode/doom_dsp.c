#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_dma.h>

#include <libdsp.h>

#include "../dspshared.h"

tDspCom dspComBuffer;
tDspCom * dspCom;

void debug(const char* msg);
short aic23_callback(void * buffer);

void main(){
    *DSP_COM=0;

    // dsp<>arm comm
    dspCom = &dspComBuffer;
	memset((void *)dspCom,0,sizeof(dspCom));
    *DSP_COM = (unsigned short) dspCom;

	// wait for ARM to finish init
	while(!dspCom->armInitFinished);
       
	// libs init
	CSL_init();
	libDsp_init(dspCom->chipNum);

	// aic23
	if (aic23_openPort()==INV) debug("Data Port NOK");
	aic23_setupDma(SAMPLECOUNT*2,aic23_callback);
	aic23_startDma();

	debug("DSP START");

	// everything is done in irq
	for(;;);
}

short aic23_callback(void * buffer){

	while(dspCom->armBusy) /* wait for arm to be ready*/;

	// request to render a buffer
	dspCom->sndWantBuf=1;
    armInt_trigger();

	while(dspCom->armBusy) /* wait for arm to finish rendering*/;

	// get data from sdram buffer
	hpiDma_start(dspCom->sndBufAddr,buffer,SAMPLECOUNT*4,HPI_DIR_SD2DSP);
	while(hpiDma_pending());

	return 1;
}

void debug(const char* msg){
	if (!dspCom) return;

	strcpy((char*)dspCom->dbgMsg,msg);
	dspCom->hasDbgMsg=1;

    armInt_trigger();

	while(dspCom->hasDbgMsg);
}
