/* 
*   cpu.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef CPU_H
#define CPU_H

#include "emu.h"

#include "mem_space.h"

#include <bkpt_list.h>

class Cpu {
    public:
        Cpu(mem_space * mem);
        
        void go(uint32_t start_address,uint32_t stack_address);
        
        void printState(void);
        void test_ini(void);
        
        /* cmd_line functions */
        int do_cmd_help(int argc,char ** argv);
        int do_cmd_quit(int argc,char ** argv);
        int do_cmd_step(int argc,char ** argv);
        int do_cmd_run(int argc,char ** argv);
        int do_cmd_add_bkpt(int argc,char ** argv);
        int do_cmd_print_stack(int argc,char ** argv);
        int do_cmd_print_state(int argc,char ** argv);
        
    private:
        uint32_t ** current_reg;
        uint32_t * mode_regs[7][18];        
        uint32_t regs_data[37];
        
        uint32_t old_PC;
        
        mem_space * mem;
        
        bkpt_list * bkpt;
               
        bool checkCondition(int condCode);
        void doThumb(uint32_t instruction);
        void doARM(uint32_t instruction);
        
        char * dispCond(int condition);
                
        void arm_MSR_MRS(int condCode,int instr_num,uint32_t instruction);
        
        void arm_DataProcessing(int condCode,uint32_t instruction);        
        void arm_LoadStore(int condCode,int instr_num,uint32_t instruction);
        void arm_LoadStoreMulti(uint32_t instruction);
        void arm_Mul(int condCode,uint32_t instruction);
        void arm_Swap(uint32_t instruction);
        void arm_CoProcessor(int condCode,uint32_t instruction);
        void arm_DSP(int condCode,uint32_t instruction);
        
        uint32_t getShifterData(uint32_t instruction,char *debugShifter,int *shifter_carry_out);
        
        uint32_t signExtend1(uint32_t data);
        uint32_t signExtend11(uint32_t data);
        uint32_t signExtend2(uint32_t data);
        
        void ARM_NegZero(uint32_t result);
        void ARM_AddCarry(uint32_t a, uint32_t b, uint32_t result);
        void ARM_AddOverflow(uint32_t a, uint32_t b, uint32_t result);
        void ARM_SubCarry(uint32_t a, uint32_t b, uint32_t result);
        void ARM_SubOverflow(uint32_t a, uint32_t b, uint32_t result);
        void ARM_AddCCarry(uint32_t a, uint32_t b, uint32_t c);
        void ARM_AddCOverflow(uint32_t a, uint32_t b, uint32_t c);
        void ARM_SubCCarry(uint32_t a, uint32_t b,uint32_t c);
        void ARM_SubCOverflow(uint32_t a, uint32_t b,uint32_t c);
        
        void thumb_data_process(int format,int opcode,uint32_t instruction);
        void thumb_load_store(int format,int opcode,uint32_t instruction);
        void thumb_load_store_multi(int format,int opcode,uint32_t instruction);
        
        
                
};

#endif
