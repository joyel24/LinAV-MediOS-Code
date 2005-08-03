#define IMMED_8(INSTR)   (INSTR&0xFF)
#define IMMED_16(INSTR)  (INSTR&0xFFFF)
#define IMMED_24(INSTR)  (INSTR&0xFFFFFF)
#define GET_RM(INSTR)    (INSTR&0xF)
#define GET_RS(INSTR)    ((INSTR>>8)&0xF)
#define GET_RD(INSTR)    ((INSTR>>8)&0xF)
#define GET_RN(INSTR)    ((INSTR>>8)&0xF)


inline uint32_t immed_32bit(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{
    shifter_operand = ((IMMED_8(instruction) << (32-GET_RS(instruction)*2)) | (IMMED_8(instruction) >> (GET_RS(instruction)*2))) ;
    if(rotate_imm == 0)
        *shifter_carry_out = (CFLAG?1:0)
    else
        *shifter_carry_out = (shifter_operand >> 31) & 0x1;
    MKDEBUG(debugShifter,"0x%x",shifter_operand);
}

inline uint32_t shift_immed_LSL(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{

}

inline uint32_t shift_immed_LSR(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{

}

inline uint32_t shift_immed_ASR(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{

}

inline uint32_t shift_immed_ROT(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{

}

inline uint32_t shift_reg_LSL(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{

}

inline uint32_t shift_reg_LSR(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{

}

inline uint32_t shift_reg_ASR(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{

}

inline uint32_t shift_reg_ROT(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{

}
