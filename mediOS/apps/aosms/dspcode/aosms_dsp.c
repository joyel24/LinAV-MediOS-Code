#include <stdio.h>
#include <string.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>

#include "../dspshared.h"
#include "libdsp.h"
#include "sn76496.h"

#define BUFFER_SIZE 1

tDspCom dspComBuffer;
tDspCom * dspCom;

static short buf1[BUFFER_SIZE];
static short buf2[BUFFER_SIZE];
static short* buffer[2]={buf1,buf2};

void debug(const char* msg);

void main(){
    MCBSP_Handle dataPort;
    int i;

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
    dataPort=aic23_openPort();
    if (dataPort==INV) debug("Data Port NOK");

    SN76496_init(3579545,SAMPLE_RATE); // 3.58Mhz
    
    debug("DSP START");

    for(;;){
		while(dspCom->psgTail!=dspCom->psgHead){
            SN76496Write(dspCom->psgQueue[dspCom->psgTail]);
			dspCom->psgTail=(dspCom->psgTail+1)&PSGQUEUE_MASK;				    		
		}  

        SN76496Update(buffer,BUFFER_SIZE,dspCom->psgStereo);

        for(i=0;i<BUFFER_SIZE;++i){
            while(!MCBSP_xrdy(dataPort));
            MCBSP_write32(dataPort,buffer[0][i]*0x10000+buffer[1][i]);
        }

        while(!(dspCom->psgEnabled));
    };
}

void debug(const char* msg){
    if (!dspCom) return;

    strcpy((char*)dspCom->dbgMsg,msg);
    dspCom->hasDbgMsg=1;

    armInt_trigger();

    while(dspCom->hasDbgMsg);
}
