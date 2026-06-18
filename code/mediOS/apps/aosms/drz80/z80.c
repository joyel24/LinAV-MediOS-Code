#include "z80.h"

#include "shared.h"

#define INT_IRQ 0x01
#define NMI_IRQ 0x02

#if 1
#define PUSH_PC() { Z80.regs.Z80SP=z80_rebaseSP(Z80.regs.Z80SP-Z80.regs.Z80SP_BASE-2); Z80.regs.z80_write16(Z80.regs.Z80PC - Z80.regs.Z80PC_BASE,Z80.regs.Z80SP - Z80.regs.Z80SP_BASE); }
#else
#define PUSH_PC() { Z80.regs.Z80SP-=2; Z80.regs.z80_write16(Z80.regs.Z80PC - Z80.regs.Z80PC_BASE,Z80.regs.Z80SP); }
#endif

/*int Z80_ICount;*/

__IRAM_DATA Z80_Regs Z80;

__IRAM_CODE static unsigned int z80_rebasePC(unsigned short address)
{
	Z80.regs.Z80PC_BASE = ((unsigned int) cpu_readmap[address >> 13])-(address&0xe000);
	Z80.regs.Z80PC =Z80.regs.Z80PC_BASE + address;
	return (Z80.regs.Z80PC);
}

__IRAM_CODE static unsigned int z80_rebaseSP(unsigned short address)
{
	Z80.regs.Z80SP_BASE=((unsigned int) cpu_readmap[address >> 13])-(address&0xe000);
	Z80.regs.Z80SP=Z80.regs.Z80SP_BASE + address;
	return (Z80.regs.Z80SP);
}

__IRAM_CODE static unsigned short z80_read16(unsigned short Addr)
{
	return (((cpu_readmem16(Addr))&0xFF)|((cpu_readmem16(Addr+1)&0xff)<<8));
}

__IRAM_CODE static void z80_write8(unsigned char Value,unsigned short Addr)
{
	cpu_writemem16(Addr,Value);
}

__IRAM_CODE static void z80_write16(unsigned short Value,unsigned short Addr)
{
	cpu_writemem16(Addr,(unsigned char)Value&0xff);
	cpu_writemem16(Addr+1,((unsigned char)(Value>>8))&0xff);
}

__IRAM_CODE static void z80_irq_callback(void)
{
	Z80.regs.Z80_IRQ = 0xff;
}

void Z80_initFctPtrsAndRebase(){
  	Z80.regs.z80_rebasePC=z80_rebasePC;
  	Z80.regs.z80_rebaseSP=z80_rebaseSP /* 0 */;
  	Z80.regs.z80_read8   =(unsigned char (*)(unsigned short))cpu_readmem16;
  	Z80.regs.z80_read16  =z80_read16;
  	Z80.regs.z80_write8  =z80_write8;
  	Z80.regs.z80_write16 =z80_write16;
  	Z80.regs.z80_in      =(unsigned char (*)(unsigned short))cpu_readport;
  	Z80.regs.z80_out     =(void (*)(unsigned short, unsigned char))cpu_writeport;
  	Z80.regs.z80_irq_callback=z80_irq_callback;
  	
  	z80_rebasePC(Z80.regs.Z80PC - Z80.regs.Z80PC_BASE);
  	z80_rebaseSP(Z80.regs.Z80SP - Z80.regs.Z80SP_BASE);
}

/****************************************************************************
 * Reset registers to their initial values
 ****************************************************************************/
void Z80_Reset(Z80_DaisyChain *daisy_chain)
{
	memset (&Z80, 0, sizeof(Z80_Regs));
    Z80_initFctPtrsAndRebase();
  	Z80.regs.Z80A = 0x00 <<24;
  	Z80.regs.Z80F = (1<<2); /* set ZFlag */
  	Z80.regs.Z80BC = 0x0000	<<16;
  	Z80.regs.Z80DE = 0x0000	<<16;
  	Z80.regs.Z80HL = 0x0000	<<16;
  	Z80.regs.Z80A2 = 0x00 <<24;
  	Z80.regs.Z80F2 = 1<<2;  /* set ZFlag */
  	Z80.regs.Z80BC2 = 0x0000 <<16;
  	Z80.regs.Z80DE2 = 0x0000 <<16;
  	Z80.regs.Z80HL2 = 0x0000 <<16;
  	Z80.regs.Z80IX = 0xFFFF	<<16;
  	Z80.regs.Z80IY = 0xFFFF	<<16;
	Z80.regs.Z80I = 0x00;
  	Z80.regs.Z80IM = 0x00;
  	Z80.regs.Z80_IRQ = 0x00;
  	Z80.regs.Z80IF = 0x00;
  	Z80.regs.Z80PC=Z80.regs.z80_rebasePC(0);
  	Z80.regs.Z80SP=Z80.regs.z80_rebaseSP(0xf000); /* 0xf000 */
	Z80.request_irq = Z80.service_irq = -1;

	Z80_Clear_Pending_Interrupts();
    	if (daisy_chain) {
        	while( daisy_chain->irq_param != -1 && Z80.irq_max < Z80_MAXDAISY )
        	{
            		/* set callbackhandler after reti */
			Z80.irq[Z80.irq_max] = *daisy_chain;
            		/* device reset */
			if( Z80.irq[Z80.irq_max].reset )
				Z80.irq[Z80.irq_max].reset(Z80.irq[Z80.irq_max].irq_param);
			Z80.irq_max++;
            		daisy_chain++;
        	}
    	}
}

void Z80_Cause_Interrupt(int type)
{
	/* type value :                                                            */
	/*  Z80_NMI_INT                      -> NMI request                        */
	/*  Z80_IGNORE_INT                   -> no request                         */
	/*  vector(0x00-0xff)                -> SINGLE interrupt request           */
	/*  Z80_VECTOR(device,status)        -> DaisyChain change interrupt status */
	/*      device : device number of daisy-chain link                         */
	/*      status : Z80_INT_REQ  -> interrupt request                         */
	/*               Z80_INT_IEO  -> interrupt disable output                  */

    	if (type == Z80_NMI_INT) {
	       	Z80.pending_irq |= NMI_IRQ;
	} else if (type != Z80_IGNORE_INT) {
        	if( Z80.irq_max ) {   /* daisy chain mode */
            		int device = type >> 8;
            		int state  = type & 0xff;
            		if( Z80.int_state[device] != state ) {

                		/* set new interrupt status */
                		Z80.int_state[device] = state;

                		/* check interrupt status */
				/* search highest interrupt request device (next interrupt device) */
				/*    and highest interrupt service device (next reti      device) */
				{
					int device;
					Z80.request_irq = Z80.service_irq = -1;

					/* search higher IRQ or IEO */
					for( device = 0 ; device < Z80.irq_max ; device ++ ) {
        					/* IEO = disable ? */
						if( Z80.int_state[device] & Z80_INT_IEO ) {
							/* if IEO is disable , masking lower IRQ */
							Z80.request_irq = -1;
							/* set highest interrupt service device */
							Z80.service_irq = device;
						}
						/* IRQ = ON ? */
						if( Z80.int_state[device] & Z80_INT_REQ )
							Z80.request_irq = device;
    					}
					/* set interrupt pending flag */
					if( Z80.request_irq >= 0 )
						Z80.pending_irq |=  INT_IRQ;
					else
						Z80.pending_irq &= ~INT_IRQ;
				}
            		}
        	} else
            {
        		/* single int mode */
			     Z80.regs.z80irqvector = type & 0xff;
            	Z80.pending_irq |= INT_IRQ;
        	}
    	}
}

void Z80_Clear_Pending_Interrupts(void)
{
	int i;
    	/* clear irq for all devices */
    	for( i = 0 ; i < Z80_MAXDAISY ; i++ )
        	Z80.int_state[i]  = 0;
    	Z80.pending_irq = 0;
	Z80.service_irq = -1;
}


/* Set all registers to given values */
void Z80_SetRegs (Z80_Regs *Regs)
{
	memcpy(&Z80,Regs,sizeof(Z80));
}

/* Get all registers in given buffer */
void Z80_GetRegs (Z80_Regs *Regs)
{
	memcpy(Regs,&Z80,sizeof(Z80));
}

/* Return program counter */
unsigned Z80_GetPC (void)
{
	return (Z80.regs.Z80PC - Z80.regs.Z80PC_BASE);
}

int Z80_GetPreviousPC (void)
{
	return (Z80.regs.previouspc);
}

#ifdef __cplusplus
extern "C" {
#endif
void Interrupt(void)
{
	/* This extra check is because DrZ80 calls this function directly but does
	    not have access to the Z80.pending_irq variable.  So we check here instead. */
	if(!Z80.pending_irq) {	return; } /* If no pending ints exit */
	
	/* Check if ints enabled */						
	if ( (Z80.pending_irq & NMI_IRQ) || (Z80.regs.Z80IF&1) )
	{

        	int irq_vector = Z80_IGNORE_INT;

		/* DrZ80 Z80IF */
		/* bit1 = _IFF1 */
		/* bit2 = _IFF2 */
		/* bit3 = _HALT */
		
        	/* Check if processor was halted */
		if (Z80.regs.Z80IF&4) 
		{
			 Z80.regs.Z80PC=Z80.regs.z80_rebasePC(Z80.regs.Z80PC - Z80.regs.Z80PC_BASE + 1);  	/* Inc PC */
			 Z80.regs.Z80IF&= ~4; 	/* and clear halt */
		}  
		 
		if (Z80.pending_irq & NMI_IRQ) 
		{
			Z80.regs.Z80IF = (Z80.regs.Z80IF&1)<<1;  /* Save interrupt flip-flop 1 to 2 and Clear interrupt flip-flop 1 */
			PUSH_PC();
			Z80.regs.Z80PC=Z80.regs.z80_rebasePC(0x0066);
			/* reset NMI interrupt request */
			Z80.pending_irq &= ~NMI_IRQ;
		}
		else 
		{
			/* Clear interrupt flip-flop 1 */
            		Z80.regs.Z80IF&= ~1;
			/* reset INT interrupt request */
			Z80.pending_irq &= ~INT_IRQ;
			if ( Z80.irq_max )
			{
                		if( Z80.request_irq >= 0 ) {
                    			irq_vector = Z80.irq[Z80.request_irq].interrupt_entry(Z80.irq[Z80.request_irq].irq_param);
                    			Z80.request_irq = -1;
                		}
            		} else
                    {
                		irq_vector = Z80.regs.z80irqvector;
            		}
            
            		/* Interrupt mode 2. Call [Z80.I:databyte] */
			if( Z80.regs.Z80IM == 2 )
			{
				irq_vector = (irq_vector & 0xff) | (Z80.regs.Z80I << 8);

                PUSH_PC();
				Z80.regs.Z80PC=Z80.regs.z80_rebasePC(Z80.regs.z80_read16(irq_vector));
			}
			else
			{
				/* Interrupt mode 1. RST 38h */
				if( Z80.regs.Z80IM == 1 )
				{
					PUSH_PC();
					Z80.regs.Z80PC=Z80.regs.z80_rebasePC(0x0038);
				} 
				else 
				{
					/* Interrupt mode 0. We check for CALL and JP instructions, */
					/* if neither of these were found we assume a 1 byte opcode */
					/* was placed on the databus */
					switch (irq_vector & 0xff0000) 
					{
						case 0xcd0000:	/* call */
							PUSH_PC();
						case 0xc30000:	/* jump */
							Z80.regs.Z80PC=Z80.regs.z80_rebasePC(irq_vector & 0xffff);
							break;
						default:
							irq_vector &= 0xff;
							PUSH_PC();
							Z80.regs.Z80PC=Z80.regs.z80_rebasePC(0x0038);
							break;
					}
				}
			}
		}
    	} else {
    		
    	}
}
#ifdef __cplusplus
} /* End of extern "C" */
#endif

/****************************************************************************
 * Execute IPeriod T-states. Return number of T-states really executed
 ****************************************************************************/
int Z80_Execute(int cycles)
{
	Z80_ICount = cycles;
	if (Z80.pending_irq)
		Interrupt();
	DrZ80Run(&Z80.regs, cycles);
	return (cycles-Z80.regs.cycles);
}

void fake_z80_freemem(void)
{
}
