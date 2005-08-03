/*
*   cpu.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <emu.h>
#include <mem_space.h>

#include <cpu.h>

#include <cmd_line.h>
#include <bkpt_list.h>

#include <HW_TI.h>

#include <gio_MAS_EOD.h>

char * mode_str[] = {"User","System","Supervisor","Abort","Undefined","IRQ","FIQ"};

enum REGS {R_R0 = 0x00, R_R1 = 0x01, R_R2 = 0x02, R_R3 = 0x03,
	   R_R4 = 0x04, R_R5 = 0x05, R_R6 = 0x06, R_R7 = 0x07,
	   R_R8 = 0x08, R_R9 = 0x09, R_R10 = 0x0A, R_FP = 0x0B,
	   R_IP = 0x0C, R_SP = 0x0D, R_LR = 0x0E, R_PC = 0x0F,
	   R_CPSR = 0x10,R_SPSR = 0x11};
           
char * reg_str[] = {"R0", "R1", "R2", "R3",
                    "R4", "R5", "R6", "R7",
                    "R8", "R9", "R10", "R11",
                    "R12", "SP", "LR", "PC",
                    "CPSR", "SPSR"};

enum MODES {M_USER=0x00,M_SYS=0x01,M_SVC=0x02,M_ABT=0x03,M_UND=0x04,M_IRQ=0x05,M_FIQ=0x06};

int cpsr_tab[16] = { M_USER , M_FIQ, M_IRQ, M_SVC,
                     0, 0, 0, M_ABT,
                     0, 0, 0, M_UND,
                     0, 0, 0, M_SYS};
                     
int mode_tab[7] = { 0x0, 0xF, 0x3, 0x7, 0xB, 0x2, 0x1};
                    

#define MODE          (cpsr_tab[*mode_regs[M_USER][R_CPSR] & 0xF])
#define SET_MODE(val) \
  { \
    *mode_regs[M_USER][R_CPSR]=(*mode_regs[M_USER][R_CPSR] & 0xFFFFFFE0) | 0x10 | mode_tab[val]; \
    current_reg = mode_regs[MODE]; \
  }

#define GET_FLAG(mask) (*mode_regs[M_USER][R_CPSR] & (mask))
#define SET_FLAG(mask) {*mode_regs[M_USER][R_CPSR] = (*mode_regs[M_USER][R_CPSR] & ~(mask)) | (mask); current_reg = mode_regs[MODE];}
#define CLR_FLAG(mask) {*mode_regs[M_USER][R_CPSR] = (*mode_regs[M_USER][R_CPSR] & ~(mask)); }

#define T_MASK        (0x00000020)
#define FIQ_MASK      (0x00000040)
#define IRQ_MASK      (0x00000080)
#define T_FLAG        GET_FLAG(T_MASK)
#define FIQ_FLAG      GET_FLAG(FIQ_MASK)
#define IRQ_FLAG      GET_FLAG(IRQ_MASK)

#define SET_T(COND)   {if(COND) SET_FLAG(T_MASK) else CLR_FLAG(T_MASK)}

#define Q_MASK        (0x08000000)
#define V_MASK        (0x10000000)
#define C_MASK        (0x20000000)
#define Z_MASK        (0x40000000)
#define N_MASK        (0x80000000)
#define Q_FLAG        GET_FLAG(Q_MASK)
#define V_FLAG        GET_FLAG(V_MASK)
#define C_FLAG        GET_FLAG(C_MASK)
#define Z_FLAG        GET_FLAG(Z_MASK)
#define N_FLAG        GET_FLAG(N_MASK)

#define SET_Q(COND)   {if(COND) SET_FLAG(Q_MASK) else CLR_FLAG(Q_MASK)}
#define SET_V(COND)   {if(COND) SET_FLAG(V_MASK) else CLR_FLAG(V_MASK)}
#define SET_C(COND)   {if(COND) SET_FLAG(C_MASK) else CLR_FLAG(C_MASK)}
#define SET_Z(COND)   {if(COND) SET_FLAG(Z_MASK) else CLR_FLAG(Z_MASK)}
#define SET_N(COND)   {if(COND) SET_FLAG(N_MASK) else CLR_FLAG(N_MASK)}

#define GET_REG(N)    ((N)==15?((*current_reg[N])&(T_FLAG?0xFFFFFFFE:0xFFFFFFFF))+(T_FLAG?2:4):*current_reg[N])
#define REG(N)        (*current_reg[N])
#define PC_REAL       (*current_reg[15])

#define RR(N)         (reg_str[N])

#define INT_DEBUG_HEAD   {printf("@%08x:%08x|%s%s%s%s%s|%s| ",old_PC,instruction, \
            N_FLAG?"N":" ",Z_FLAG?"Z":" ",C_FLAG?"C":" ",V_FLAG?"V":" ",Q_FLAG?"Q":" ", \
            cond_str[condCode&0xF]);}
#define INT_DEBUG_HEAD_THUMB   {printf("@%08x:%04x|%s%s%s%s%s| ",old_PC,instruction, \
            N_FLAG?"N":" ",Z_FLAG?"Z":" ",C_FLAG?"C":" ",V_FLAG?"V":" ",Q_FLAG?"Q":" ");}
            
#ifdef DEBUG_MODE
#define DEBUG_HEAD         if(run_mode==STEP || disp_mode==1) INT_DEBUG_HEAD
#define DEBUG_HEAD_THUMB   if(run_mode==STEP || disp_mode==1) INT_DEBUG_HEAD_THUMB
#else
#define DEBUG_HEAD
#define DEBUG_HEAD_THUMB
#endif

#define TST_BIT(V,N)   (((V)>>(N))&0x1)
           
#define CHG_MODE     {   int __old_mode=MODE;      \
                         REG(R_CPSR)=REG(R_SPSR);  \
                         if(__old_mode != MODE)    \
                         { \
                            printf("Mode has changed from %s to %s at %x\n",mode_str[__old_mode],mode_str[MODE],PC_REAL); \
                            current_reg = mode_regs[MODE]; \
                         } \
                     }

int run_mode;
int HW_mode=0;
int disp_mode=0;
                     
uint32_t old_PC;
                   
char * cond_str[] = {"EQ","NE","CS","CC","MI","PL","VS","VC","HI","LS","GE","LT","GT","LE","  ","ERR"};

uint32_t ** current_reg;
uint32_t * mode_regs[7][18];        
uint32_t regs_data[37];

uint32_t my_data;

extern mem_space * mem;

bkpt_list * bkpt;

bool data_abort=false;

void init_cpu_static_fct(void);

#define new_thumb

#ifdef new_thumb

#include "thumb_data_processing_new.h"
#include "thumb_load_store_new.h"
#include "thumb_load_store_multi_new.h"
#include "thumb_misc_new.h"

struct fct_data {
    int blank_size;
    int mask;    
    void (*fct)(uint32_t instruction);
};

void (*thumb_fct[1024])(uint32_t ) ;

#include "thumb_fct_ini_tab.h"

void ini_thumb_fct(void)
{
    int i,j,k;
    /* clearing the whole fct array */
    for(i=0;i<1024;i++)
        thumb_fct[i] = NULL;
        
    /* init array using fct_ini_tab */    
    for(i=0;fct_ini_tab[i].fct!=NULL;i++)
    {
        j=0x1<<fct_ini_tab[i].blank_size;
        for(k=0;k<j;k++)
        {
            printf("%04d |Processing MASK:%x POS:%x/%x val:%x\n",
                i,fct_ini_tab[i].mask,k,fct_ini_tab[i].blank_size,fct_ini_tab[i].mask|k);
            if(thumb_fct[fct_ini_tab[i].mask|k])
                printf("error thumb_fct[%x] already defined (cur %x)\n",fct_ini_tab[i].mask|k,i);
            else
                thumb_fct[fct_ini_tab[i].mask|k] = fct_ini_tab[i].fct;
        }
    }
    
    /*Let's see if we forget something*/
    k=0;
    for(j=0;j<1024;j++)
        if(!thumb_fct[j])
        {
            k++;
            printf("%04d|error thumb_fct[%x] not defined\n",k,j);            
        }
    printf("[ini_thumb fct] Processed %04d fct, we've missed : %04d cells in thumb_fct\n",i,k);
}

#endif

                     
void init_cpu(void)
{
    int i,j;
    
    /*init regs data*/
    for(i=0;i<38;i++)
        regs_data[i]=0;

           
    /*ini the same regs for all modes */
    for(i=0;i<17;i++)
        for(j=0;j<7;j++)
            mode_regs[j][i] = &regs_data[i];
    
    /* SVC mode */
    mode_regs[M_SVC][R_SP] =  &regs_data[17];
    mode_regs[M_SVC][R_LR] =  &regs_data[18];
    mode_regs[M_SVC][R_SPSR] = &regs_data[19];
    
    /* ABT mode */
    mode_regs[M_ABT][R_SP] =  &regs_data[20];
    mode_regs[M_ABT][R_LR] =  &regs_data[21];
    mode_regs[M_ABT][R_SPSR] = &regs_data[22];
    
    /* UND mode */
    mode_regs[M_UND][R_SP] =  &regs_data[23];
    mode_regs[M_UND][R_LR] =  &regs_data[24];
    mode_regs[M_UND][R_SPSR] = &regs_data[25];
    
    /* IRQ mode */
    mode_regs[M_IRQ][R_SP] =  &regs_data[26];
    mode_regs[M_IRQ][R_LR] =  &regs_data[27];
    mode_regs[M_IRQ][R_SPSR] = &regs_data[28];
    
    /* FIQ mode */
    for(i=0;i<7;i++)
        mode_regs[M_FIQ][R_R8+i] = &regs_data[29+i];
    mode_regs[M_FIQ][R_SPSR] = &regs_data[36];
       
    SET_FLAG(IRQ_MASK);
    SET_FLAG(FIQ_MASK);
    
    SET_MODE(INIT_MODE); //M_SYS
    
    /* init the cmd line */
    
    init_cpu_static_fct();
    
    /* init bkpt_list */
    
    bkpt= new bkpt_list(); 
    
#ifdef new_thumb
    ini_thumb_fct();
#endif
    printf("Init of Cpu object      DONE\n");
    
}

void sigint(void)
{
    run_mode = STEP;
}

#include "cpu_cmd_line_fct.h"

void go(uint32_t start_address,uint32_t stack_address)
{
    uint32_t instruction;
    REG(R_PC)=start_address;
    REG(R_SP)=stack_address;
    
    run_mode = STEP;
    uint32_t address;
    
    while(1)
    {  
        mem->hw_TI->uart_list[0]->nxtEvent();
        mem->hw_TI->uart_list[1]->nxtEvent();
        mem->hw_TI->osd->nxtEvent();
        
        ((gio_MAS_EOD*)mem->hw_TI->gpio->port_list[0x4])->chkEOD();
        
        for(int i=0;i<4;i++)            
            mem->hw_TI->timer_list[i]->nxt_cycle();    
        
        #if 0
        if(data_abort)
        {
            data_abort = false;
            *mode_regs[M_ABT][R_LR]=PC_REAL+8;
            *mode_regs[M_ABT][R_SPSR]=REG(R_CPSR);
            SET_MODE(M_ABT);
            CLR_FLAG(T_MASK);
            SET_FLAG(IRQ_MASK);
            REG(R_PC)=0x10;
            //printf("WARNING DATA ABT\n");
            run_mode = STEP;
        }
        #endif
             
        if(!FIQ_FLAG && mem->hw_TI->HW_irq->have_int_FIQ)
        {
            mem->hw_TI->HW_irq->have_int_FIQ = false;
            //printf("FIQ - return at %x\n",PC_REAL+4);
            *mode_regs[M_FIQ][R_LR]=PC_REAL+4;
            *mode_regs[M_FIQ][R_SPSR]=REG(R_CPSR);
            SET_MODE(M_FIQ);
            CLR_FLAG(T_MASK);
            SET_FLAG(FIQ_MASK);
            SET_FLAG(IRQ_MASK);
            REG(R_PC)=0x1C;
        }
        
        if(!IRQ_FLAG && mem->hw_TI->HW_irq->have_int_IRQ)
        {
            //printState();
            mem->hw_TI->HW_irq->have_int_IRQ = false;
            //printf("IRQ - return at %x\n",PC_REAL+4);
            *mode_regs[M_IRQ][R_LR]=PC_REAL+4;
            *mode_regs[M_IRQ][R_SPSR]=REG(R_CPSR);
            SET_MODE(M_IRQ);
            CLR_FLAG(T_MASK);
            SET_FLAG(IRQ_MASK);
            REG(R_PC)=0x18;
        }
        
        address = T_FLAG ? PC_REAL&0xfffffffe : (PC_REAL+2)&0xfffffffc;
        
        if(bkpt->has_bkpt(address,BKPT_CPU))
        {
            run_mode = STEP;
            cmd_line();
        }
        else if(run_mode == STEP)
        {
            cmd_line();
        }
                
        old_PC=PC_REAL;
        if(T_FLAG)  /* THUMB */
        {        
            instruction=mem->read(address,2);
            PC_REAL+=2;
#ifdef new_thumb
            DEBUG_HEAD_THUMB;
            thumb_fct[(instruction>>6)&0x3FF](instruction);
            if(disp_mode==1 || run_mode==STEP)
                printState();
#else
            doThumb(instruction);
#endif
        }
        else       /* ARM */
        {
            instruction=mem->read(address,4);
            PC_REAL+= 4;
            doARM(instruction);
        }
        
        
        
    }
}

bool checkCondition(int condCode)
{
    switch(condCode & 0xF)
    {
            case 0xE:                                       // AL
                    return true;
            case 0x0:                                       // EQ
                    return Z_FLAG;
            case 0x1:                                       // NE
                    return !Z_FLAG;
            case 0x2:                                       // CS/HS
                    return C_FLAG;
            case 0x3:                                       // CC/LO
                    return !C_FLAG;
            case 0x4:                                       // MI
                    return N_FLAG;
            case 0x5:                                       // PL
                    return !N_FLAG;
            case 0x6:                                       // VS
                    return V_FLAG;
            case 0x7:                                       // VC
                    return !V_FLAG;
            case 0x8:                                       // HI
                    return (C_FLAG && !Z_FLAG);
            case 0x9:                                       // LS
                    return (!C_FLAG || Z_FLAG);
            case 0xA:                                       // GE
                    return (N_FLAG == V_FLAG);
            case 0xB:                                       // LT
                    return (N_FLAG != V_FLAG);
            case 0xC:                                       // GT
                    return (!Z_FLAG && ((N_FLAG && V_FLAG) || (!N_FLAG && !V_FLAG)));
            case 0xD:                                       // LE
                    return (Z_FLAG || ((N_FLAG && !V_FLAG) || (!N_FLAG && V_FLAG)));
            case 0xF:                                       // Error
                    printf("Error cond code = 0xF (b1111)");
                    return false;
    }
    return false;
}

void doARM(uint32_t instruction)
{
    int condCode = (instruction >> 28) & 0xf;
    int instr_num = (instruction >> 25) & 0x7;
    
    DEBUG_HEAD;
    
    switch(instr_num)
    {
        case 0x0:
            if((instruction>>4)&0x1)
            {                           /* BIT 4 == 1 */
                
                if((instruction>>7)&0x1)
                {                           /* BIT 7 == 1 */
                    if(((instruction>>5)&0x1)==0 && ((instruction>>6)&0x1)==0)
                    {                           /* Multiply / swap */
                        if(((instruction>>24)&0x1)==0)
                        {                           /* BIT 24 == 1 */ /* Multiply */
                            arm_Mul(condCode,instruction);
                        }
                        else
                        {                           /* BIT 24 == 0 */ /* Swap */
                            if(checkCondition(condCode))
                                arm_Swap(instruction);
                            else
                                DEBUG("SWP CC not met\n");
                        }
                    }                            /* Multiply / swap */
                    else
                    {                            /* extra Load/Store */
                        arm_LoadStore(condCode,instr_num,instruction);
                    }                            /* extra Load/Store */
                    
                }                           /* BIT 7 == 1 */
                else
                {                           /* BIT 7 == 0 */ /* Multiplies extra Load/Store */
                    if((instruction & 0x01900000) == 0x01000000) /* MISC instructions */
                    {
                        switch((instruction>>5)&0x3)
                        {
                            case 0:
                                if((instruction>>22)&0x1)
                                {                           /* BIT 22 == 1 */ /* CLZ */
                                    INT_DEBUG_HEAD
                                    printf("CLZ : Undef instruction\n");
                                    exit(0);
                                }
                                else
                                {                           /* BIT 22 == 0 */ /* BX */
                                    if(checkCondition(condCode))
                                    {
                                        DEBUG("BX ");
                                        
                                        int Rm = instruction & 0xF;
                                        
                                        if(GET_REG(Rm) & 0x1)
                                        {
                                            SET_FLAG(T_MASK);
                                            DEBUG("Thumb - ");
                                        }
                                        else
                                        {
                                            CLR_FLAG(T_MASK);
                                            DEBUG("ARM - ");
                                        }
                                        REG(R_PC) = GET_REG(Rm) & 0xFFFFFFE;
                                        DEBUG("=> %08x\n",GET_REG(R_PC));
                                    }
                                    else
                                    {
                                        DEBUG("BX CC not met\n");
                                    }
                                }
                                break;
                            case 1:
                                INT_DEBUG_HEAD
                                printf("BLX: Undef instruction\n");
                                exit(0);
                                break;
                            case 2:
                                arm_DSP(condCode,instruction);
                                break;
                            case 3:
                                INT_DEBUG_HEAD
                                printf("BKPT: Undef instruction\n");
                                exit(0);
                                break;
                        }
                    }
                    else                                         /* data processing register shift*/
                        arm_DataProcessing(condCode,instruction);
                }                           /* BIT 7 == 0 */ /* Multiplies extra Load/Store */
            }                           /* BIT 4 == 1 */
            else
            {                           /* BIT 4 == 0 */
                if((instruction & 0x01900000) == 0x01000000) /* MISC instructions */
                {
                    if((instruction>>7)&0x1)    /* BIT 7 == 1 */ /* DSP mul */
                        arm_DSP(condCode,instruction);
                    else                        /* BIT 7 == 0 */ /* MSR/MRS */
                        arm_MSR_MRS(condCode,instr_num,instruction); 
                }
                else                                         /* data processing immediate shift*/
                    arm_DataProcessing(condCode,instruction);
            }                           /* BIT 4 == 0 */
            break;
        case 0x1:
            if((instruction & 0x01900000) == 0x01000000)
                if((instruction>>21)&0x1)   /* Move immediate to status reg */
                    arm_MSR_MRS(condCode,instr_num,instruction);                    
                else
                {
                    INT_DEBUG_HEAD
                    printf("Undef instruction (1) %x\n",instruction);                   
                    exit(0);
                }
            else   /* data processing immediate */
                arm_DataProcessing(condCode,instruction);
            break;
        case 0x3:
            if(condCode == 0xf)
            {
                INT_DEBUG_HEAD
                printf("Undef instruction (2) %x\n",instruction);
                exit(0);
            }
        case 0x2:
            arm_LoadStore(condCode,instr_num,instruction);
            break;
        case 0x4:
            if(checkCondition(condCode))
                arm_LoadStoreMulti(instruction);
            else
                DEBUG("Load/Store multi CC not met\n");
            break;
        case 0x5:            
            if(checkCondition(condCode))       /* B, BL */
            {   
                if ((instruction>>24)&0x1)
                {
                    REG(R_LR) = PC_REAL;
                    DEBUG("BL => ");
                }
                else
                    DEBUG("B => ");
    
                uint32_t offset = (instruction & 0xffffff);
                
                if (offset > 0x7fffff)
                    offset = offset - 0x1000000 ;
                
                REG(R_PC) = GET_REG(R_PC) + offset *4;
                
                DEBUG("%08x\n",PC_REAL);
            }
            else
            {
                DEBUG("B/BL CC not met\n");
            }
            break;
        case 0x6:
            if(checkCondition(condCode))
                arm_CoProcessor(condCode,instruction);
            else
                DEBUG("CoProcessor CC not met\n");
            break;
        case 0x7:
            if(((instruction>>24)&0x1)==0)
            {
                if(checkCondition(condCode))
                    arm_CoProcessor(condCode,instruction);
                else
                    DEBUG("CoProcessor CC not met\n");
            }
            else
            {
                if(checkCondition(condCode))
                {
                    DEBUG("SWI :%08x\n",instruction & 0x00FFFFFF);
                    *mode_regs[M_SVC][R_LR]=PC_REAL;
                    *mode_regs[M_SVC][R_SPSR]=REG(R_CPSR);
                    SET_MODE(M_SVC);
                    CLR_FLAG(T_MASK);
                    SET_FLAG(IRQ_MASK);
                    REG(R_PC)=0x8;
                }
                else
                    DEBUG("SWI CC not met\n");
            }
            break;
         default:
             printf("You should not be here\n");
             exit(0);
    }
    if(disp_mode==1 || run_mode==STEP)
        printState();
}

void arm_MSR_MRS(int condCode,int instr_num,uint32_t instruction)
{
    if (checkCondition(condCode))
    {   
        int old_mode = MODE;
        
        if((instruction >> 21) & 0x1)           /* MSR */
        {
            uint32_t  opVal;
            int Rm=instruction&0xF;
            DEBUG("MSR ");            
            
            if((instruction>>25) & 0x1)
            {
                opVal=((instruction&0xFF) << (32-(((instruction>>8) & 0xF)*2))) | ((instruction&0xFF) >> (((instruction>>8) & 0xF)*2));
                DEBUG("[%08x] -> ",opVal);
            }
            else
            {
                opVal = GET_REG(Rm);
                DEBUG("R%d (%08x) -> ",Rm,GET_REG(Rm));
            }
            int mask = (instruction >> 16) & 0xF;
            if((instruction >> 22) & 0x1)
            {
                
                DEBUG("SPSR (mask =%08x)",mask);
                if(old_mode == M_USER || old_mode == M_SYS)
                {
                    DEBUG(" !! ERROR, no SPSR in %s mode\n",mode_str[old_mode]);
                }
                else
                {
                    if( (mask & 0x1) == 0x1)
                        REG(R_SPSR) = (GET_REG(R_SPSR) & ~0x000000FF) | (opVal & 0x000000FF);
                    if( (mask & 0x2) == 0x2)
                        REG(R_SPSR) = (GET_REG(R_SPSR) & ~0x0000FF00) | (opVal & 0x0000FF00);
                    if( (mask & 0x4) == 0x4)
                        REG(R_SPSR) = (GET_REG(R_SPSR) & ~0x00FF0000) | (opVal & 0x00FF0000);
                    if( (mask & 0x8) == 0x8)
                        REG(R_SPSR) = (GET_REG(R_SPSR) & ~0xFF000000) | (opVal & 0xFF000000);
                    DEBUG(" => %08x\n",GET_REG(R_SPSR));
                }
            }
            else
            {
                DEBUG("CPSR (%08x)",mask);
                uint32_t R_CPSR_SAV=GET_REG(R_CPSR);
                uint32_t R_CPSR_CUR;
                
                if( (mask & 0x1) == 0x1)
                {
                    REG(R_CPSR) = (GET_REG(R_CPSR) & ~0x000000FF) | (opVal & 0x000000FF);
                    R_CPSR_CUR=GET_REG(R_CPSR);
                    DEBUG(" => %08x\n",GET_REG(R_CPSR));
                    DEBUG_HW(CPU_DEBUG,"%s",((R_CPSR_SAV&0x80) == (R_CPSR_CUR&0x80)) ? "" : (R_CPSR_CUR&0x80)?"DISABLE IRQ\n":"ENABLE IRQ\n");
                    DEBUG_HW(CPU_DEBUG,"%s",((R_CPSR_SAV&0x40) == (R_CPSR_CUR&0x40)) ? "" : (R_CPSR_CUR&0x40)?"DISABLE FIQ\n":"ENABLE FIQ\n");
                }
                else
                {
                    if( (mask & 0x2) == 0x2)
                        REG(R_CPSR) = (GET_REG(R_CPSR) & ~0x0000FF00) | (opVal & 0x0000FF00);
                    if( (mask & 0x4) == 0x4)
                        REG(R_CPSR) = (GET_REG(R_CPSR) & ~0x00FF0000) | (opVal & 0x00FF0000);
                    if( (mask & 0x8) == 0x8)
                        REG(R_CPSR) = (GET_REG(R_CPSR) & ~0xFF000000) | (opVal & 0xFF000000);
                    DEBUG(" => %08x\n",GET_REG(R_CPSR));
                }
                
                
            }
        }
        else                                   /* MRS */
        {
            DEBUG("MRS ");
            
            int Rd = (instruction >> 12) & 0xF;
            if((instruction >> 22) & 0x1)             /* SPSR */
            {
                 if(old_mode == M_USER || old_mode == M_SYS)
                {
                    DEBUG("ERROR, no SPSR in %s mode\n",mode_str[old_mode]);
                    
                }
                else
                {
                    DEBUG("R%d <- SPSR (%08x)\n",Rd,GET_REG(R_SPSR));
                    REG(Rd) = GET_REG(R_SPSR);
                }
                
            }
            else                                      /* CPSR */
            {
                DEBUG("R%d <- CPSR (%08x)\n",Rd,GET_REG(R_CPSR));
                REG(Rd) = GET_REG(R_CPSR);
            }
        }
        
        /* checking mode change */
        if(old_mode != MODE)
        {
            DEBUG("Mode has changed from %s to %s\n",mode_str[old_mode],mode_str[MODE]);
            current_reg = mode_regs[MODE];
        }
    }
    else
    {
        DEBUG("MSR/MRS CC not met\n");
    }
    
    
            
}

/* special ops */

#include "cpu_ops.h"

/* load store */

#include "data_processing.h"

/* load store */

#include "cpu_load_store.h"

/* load store MULTI */

#include "cpu_load_store_multi.h"

/* multiply */

#include "cpu_multiply.h"

       
void arm_CoProcessor(int condCode,uint32_t instruction)
{
    INT_DEBUG_HEAD
    printf("coprocessor instruction: %08x\n",instruction);
    exit(0);
}


void arm_DSP(int condCode,uint32_t instruction)
{
    INT_DEBUG_HEAD
    printf("DSP instruction: %08x\n",instruction);
    exit(0);
}

void arm_Swap(uint32_t instruction)
{
    int Rm=instruction & 0xF;
    int Rn=(instruction>>16) &0xF;
    int Rd=(instruction>>12) &0xF;
    
    uint32_t data;  
    
    DEBUG("SWP"); 

    if((instruction>>22) & 0x1)           // SWPB
    {
        DEBUG("B [R%d] = R%d R%d = [R%d]\n",Rn,Rm,Rd);
        data = mem->read(GET_REG(Rn),1);
        mem->write(GET_REG(Rn),GET_REG(Rm) & 0xFF,1);
        REG(Rd) = data;
    }
    else                                  // SWP
    {
        DEBUG(" [R%d] = R%d R%d = [R%d] (rot right:",Rn,Rm,Rd);
        switch(GET_REG(Rn) & 0x3)
        {
            case 0x0:
                data = mem->read(GET_REG(Rn),4);
                DEBUG("0)\n");
                break;
            case 0x1:
                data = mem->read(GET_REG(Rn),4);
                data = ((data << 24) & 0xFF000000) | ((data >> 8) & 0x00FFFFFF);
                DEBUG("8)\n");
                break;
            case 0x2:
                data = mem->read(GET_REG(Rn),4);
                data = ((data << 16) & 0xFFFF0000)  | ((data >> 16) & 0x0000FFFF);
                DEBUG("16)\n");
                break;
            case 0x3:
                data = mem->read(GET_REG(Rn),4);
                data = ((data << 8) & 0xFFFFFF00)  | ((data >> 24) & 0xFF000000);
                DEBUG("24)\n");
                break;
        }
        mem->write(GET_REG(Rn),GET_REG(Rm),4);
        REG(Rd)=data;
    }
}

void doThumb(uint32_t instruction)
{
    int instr_num = (instruction >> 13) & 0x7;
    
    DEBUG_HEAD_THUMB;
    
    switch(instr_num)
    {
        case 0x0:
            switch((instruction>>10)&0x7)
            {
                case 0x6:
                    thumb_data_process(0x0,(instruction>>9)&0x1,instruction);
                    break;
                case 0x7:
                    thumb_data_process(0x1,(instruction>>9)&0x1,instruction);
                    break;
                default:
                    thumb_data_process(0x3,(instruction>>11)&0x3,instruction);
                    break;
            }
            break;
        case 0x1:
            thumb_data_process(0x2,(instruction>>11)&0x3,instruction);
            break;
        case 0x2:
            if(TST_BIT(instruction,12) == 0)
            {
                if(TST_BIT(instruction,11) == 0)
                {
                    if(TST_BIT(instruction,10) == 0)
                    {
                        thumb_data_process(0x4,(instruction>>6)&0xF,instruction);
                    }
                    else
                    {
                        if(((instruction>>8)&0x3)!=0x3)
                        {
                            thumb_data_process(0x7,(instruction>>8)&0x3,instruction);
                        }
                        else
                        {
                            if(TST_BIT(instruction,7) == 1)
                            {
                                INT_DEBUG_HEAD_THUMB
                                printf("BLX undefined in armV4\n");
                                exit(0);
                            }
                            else                    /* BX */
                            {
                                int Rm = (instruction>>3)&0xF;
                                uint32_t dest = GET_REG(Rm) & 0xFFFFFFFE;
                                SET_T(GET_REG(Rm) & 0x1);
                                REG(R_PC) = dest;
                                DEBUG("BX %s => 0x%08x\n",GET_REG(Rm) & 0x1?"THUMB":"ARM",dest);
                            }
                        }
                    }
                }
                else
                {
                    thumb_load_store(0x2,0x0,instruction);
                }
            }
            else
            {
                thumb_load_store(0x1,(instruction>>9)&0x7,instruction);
            }
            break;
        case 0x3:
            thumb_load_store(0x0,(instruction>>11)&0x1F,instruction);
            break;
        case 0x4:
            if(TST_BIT(instruction,12) == 0)
                thumb_load_store(0x0,(instruction>>11)&0x1F,instruction);
            else
                thumb_load_store(0x3,(instruction>>11)&0x1,instruction);
            break;
        case 0x5:
            if(TST_BIT(instruction,12) == 0)
            {
                thumb_data_process(0x5,0,instruction);
            }
            else
            {
                switch((instruction>>9)&0x3)
                {
                    case 0x0:
                        thumb_data_process(0x6,(instruction>>7)&0x1,instruction);
                        break;
                    case 0x2:
                        thumb_load_store_multi(0x1,(instruction>>11)&0x1,instruction);
                        break;
                    case 0x3:
                        INT_DEBUG_HEAD_THUMB
                        printf("BKT undef for ARMv4\n");
                        exit(0);
                    default:
                        INT_DEBUG_HEAD_THUMB
                        printf("Error: MISC instruction with undef op: %x\n",(instruction>>9)&0x3);
                        exit(0);
                }                
            }
            break;
        case 0x6:
            if(TST_BIT(instruction,12) == 0)
            {
                thumb_load_store_multi(0x0,(instruction>>11)&0x1,instruction);
            }
            else
            {
                int cond=(instruction>>8)&0xF;
                switch(cond)
                {
                    case 0xF:                            /* SWI */
                        DEBUG("SWI :%08x\n",instruction & 0x000000FF);
                        *mode_regs[M_SVC][R_LR]=PC_REAL;
                        *mode_regs[M_SVC][R_SPSR]=REG(R_CPSR);
                        SET_MODE(M_SVC);
                        CLR_FLAG(T_MASK);
                        SET_FLAG(IRQ_MASK);
                        REG(R_PC)=0x8;
                        break;
                    case 0xE:
                        INT_DEBUG_HEAD_THUMB
                        printf("undefined instruction (3) %x\n",instruction);
                        exit(0);
                        break;
                    default:                             /* B <cond> */
                        DEBUG("B<%s> ",cond_str[cond&0xF]);
                        if(checkCondition(cond))
                        {
                            uint32_t offset=(instruction&0xFF);
                            offset=signExtend1(offset)<<1;
                            REG(R_PC) = GET_REG(R_PC) + offset;
                            DEBUG("=> 0x%08x\n",PC_REAL);                            
                        }
                        else
                            DEBUG("CC not met\n");
                        break;
                        
                }
            }
            break;
        case 0x7:
                uint32_t offset = instruction & 0x7FF;
                switch((instruction>>11)&0x3)
                {
                    
                    case 0x0:                /* B */
                        offset=signExtend11(offset)<<1;
                        REG(R_PC)=GET_REG(R_PC) + offset;
                        DEBUG("B => 0x%08x\n",PC_REAL);                        
                        break;
                    case 0x1:                /* BLX step 2 => not defined */
                        INT_DEBUG_HEAD_THUMB
                        printf("BLX undefined instruction\n");
                        exit(0);
                        break;
                    case 0x2:                /* BL(X) step 1 */
                        offset=signExtend11(offset)<<12;
                        REG(R_LR)=GET_REG(R_PC)+offset;
                        DEBUG("BL stp1 off 0x%08x\n",offset);
                        break;
                    case 0x3:                /* BL step 2 */
                        offset=GET_REG(R_LR)+(offset<<1);
                        REG(R_LR) = PC_REAL | 1 ;
                        REG(R_PC) = offset;
                        DEBUG("BL => 0x%08x\n",offset);
                        break;
                }
            break;
    }
    if(disp_mode==1 || run_mode==STEP)
        printState();
}

/* thumb data processing */

#include "thumb_data_processing.h"

/* thumb load/store */

#include "thumb_load_store.h"

/* thumb load/store multi*/

#include "thumb_load_store_multi.h"

////////////////////////////// signExtend
uint32_t signExtend1(uint32_t data)
{
    if(data > 0x7F) // neg
        data -=  0x100;
    return data;
}

uint32_t signExtend11(uint32_t data)
{
    if(data > 0x3FF) // neg
    {
        data -=  0x800;
    }
    return data;
}

uint32_t signExtend2(uint32_t data)
{
    if(data> 0x7FFF) // neg
        data -=  0x10000;
    return data;
}

void printState(void)
{
    int i=0;
    printf("Mode : %s (%d) - PC:%08x, LR:%08x, SP:%08x\n",mode_str[MODE],MODE,
            GET_REG(R_PC),
            GET_REG(R_LR),
            GET_REG(R_SP));
    for(i=0;i<5;i++)
        printf("R%d:%08x ",i,GET_REG(i));
    printf("\n");
    
    for(i;i<9;i++)
        printf("R%d:%08x ",i,GET_REG(i));
    printf("\n");
    for(i;i<13;i++)
        printf("R%d:%08x ",i,GET_REG(i));
    printf("\n");
    
    printf("CPSR: %08x\n",GET_REG(R_CPSR));
    
    if(MODE != M_USER && MODE != M_SYS)
        printf("SPSR: %08x\n",GET_REG(R_SPSR));
    
}
