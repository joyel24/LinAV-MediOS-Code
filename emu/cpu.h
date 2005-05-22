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

#include "memory.h"


class Cpu {
    public:
        Cpu(Memory * mem);
        
        void go(uint32_t start_address,uint32_t stack_address);
        
        void printState(void);
        void test_ini(void);
        
    private:
        uint32_t ** current_reg;
        uint32_t * mode_regs[7][18];        
        uint32_t regs_data[37];
        
        uint32_t old_PC;
        
        Memory * mem;
        
        bool checkCondition(int condCode);
        void doThumb(uint32_t instruction);
        void doARM(uint32_t instruction);
        
        void arm_MSR_MRS(int condCode,int instr_num,uint32_t instruction);
        
        void arm_DataProcessing(int condCode,int instr_num,uint32_t instruction);        
        void arm_LoadStore(int condCode,int instr_num,uint32_t instruction);
        void arm_LoadStoreMulti(uint32_t instruction);
        void arm_Mul(int condCode,uint32_t instruction);
        void arm_Swap(uint32_t instruction);
        void arm_CoProcessor(uint32_t instruction);
        void arm_DSP(int condCode,uint32_t instruction);
        
};

#endif
