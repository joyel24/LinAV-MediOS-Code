#define GET_R1  (instruction&0x7)
#define GET_R2  ((instruction>>3)&0x7)
#define GET_R3  ((instruction>>6)&0x7)
#define GET_LR1 ((instruction & 0x7) | ((instruction & 0x0080) >> 4))
#define GET_LR2 (((instruction >> 3) & 0x7) | ((instruction & 0x0040) >> 3))

void thumb_adc(uint32_t instruction) /* format 4 */
{
    uint32_t op1 = GET_REG(GET_R1);
    int addOn=C_FLAG?1:0;
    uint32_t op2 = GET_REG(GET_R2);
    DEBUG("ADC %s, %s\n",RR(GET_R1),RR(GET_R2));                    
    REG(GET_R1) = op1 + op2 + addOn;
    ARM_NegZero(GET_REG(GET_R1));
    ARM_AddCCarry(op1,op2,addOn);
    ARM_AddCOverflow(op1,op2,addOn);
}

#define DO_ADD(REG_N,OP1,OP2) {             \
    REG(REG_N) = OP1 + OP2;                 \
    ARM_NegZero(GET_REG(REG_N));            \
    ARM_AddCarry(OP1,OP2, OP1 + OP2);       \
    ARM_AddOverflow(OP1,OP2, OP1 + OP2);    \
}

void thumb_add_3Reg(uint32_t instruction) /* format 0 */
{
    uint32_t op1 = GET_REG(GET_R2);
    uint32_t op2 = GET_REG(GET_R3);
    DEBUG("ADD %s, %s, %s\n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    DO_ADD(GET_R1,op1,op2)
}

void thumb_add_2Reg_imm(uint32_t instruction) /* format 1 */
{
    uint32_t op1 = GET_REG(GET_R2);
    uint32_t op2 = GET_R3;
    DEBUG("ADD %s, %s, 0x%x\n",RR(GET_R1),RR(GET_R2),op2);
    DO_ADD(GET_R1,op1,op2)
}

void thumb_add_Reg_imm(uint32_t instruction) /* format 2 */
{
    
    uint32_t op1 = GET_REG(((instruction>>8)&0x7));
    uint32_t op2 = (instruction&0xFF);
    DEBUG("ADD %s, 0x%x\n",RR(((instruction>>8)&0x7)),op2);
    DO_ADD(((instruction>>8)&0x7),op1,op2)
}

void thumb_add_Reg_PC_imm(uint32_t instruction) /* format 5/PC */
{
    uint32_t op2 = (instruction&0xFF)<<2;
    DEBUG("ADD %s, PC, 0x%x\n",RR(((instruction>>8)&0x7)),op2);
    REG(((instruction>>8)&0x7)) = (GET_REG(R_PC) & 0xFFFFFFFC) + op2;
}

void thumb_add_Reg_SP_imm(uint32_t instruction) /* format 5/SP */
{
    uint32_t op2 = (instruction&0xFF)<<2;
    DEBUG("ADD %s, SP, 0x%x\n",RR(((instruction>>8)&0x7)),op2);
    REG(((instruction>>8)&0x7)) = GET_REG(R_SP) + op2;
}

void thumb_add_SP_imm(uint32_t instruction) /* format 6 */
{
    uint32_t op2 = (instruction&0x7F)<<2;
    DEBUG("ADD SP, 0x%x\n",op2);
    REG(R_SP) = GET_REG(R_SP) + op2;
}

void thumb_add_2LongR(uint32_t instruction) /* format 7 */
{
    DEBUG("ADD %s, %s\n",RR(GET_LR1),RR(GET_LR2));
    REG(GET_LR1) = GET_REG(GET_LR1) + GET_REG(GET_LR2);
}


void thumb_and(uint32_t instruction) /* format 4 */
{
    DEBUG("AND %s, %s\n",RR(GET_R1),RR(GET_R2));
    REG(GET_R1)=GET_REG(GET_R1) & GET_REG(GET_R2);
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_asr_2Reg(uint32_t instruction) /* format 4 */
{
    uint32_t op1 = GET_REG(GET_R1);
    uint32_t op2 = GET_REG(GET_R2) & 0xFF;
    
    DEBUG("ASR %s, %s\n",RR(GET_R1),RR(GET_R2));
    
    if(op2 > 0 )
    {
        if(op2 <32)
        {
            SET_C((op1 >> (op2 - 1)) & 0x1);
            if(op1 > 0x7FFFFFFF) // neg number => keep neg sign
            {
                for (int j = 0; j < op2; j++) {
                        op1 = (op1 >> 1) | 0x80000000;
                }
                REG(GET_R1) = op1;
            }
            else
                REG(GET_R1) = op1 >> op2;
        }
        else 
        {
            SET_C((op1 >> 31) & 0x1);
            if(!C_FLAG)
                REG(GET_R1) = 0;
            else
                REG(GET_R1) = 0xFFFFFFFF;
        }
    }
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_asr_2Reg_imm(uint32_t instruction) /* format 3 */
{
    uint32_t op1 = GET_REG(GET_R2);
    uint32_t op2 = (instruction >> 6) & 0x1F;
    
    DEBUG("ASR %s, %s, 0x%x",RR(GET_R1),RR(GET_R2),op2);
    
    
    if(op2 == 0)
    {
        SET_C((op1 >> 31) & 0x1);
        if(!C_FLAG)
            REG(GET_R1) = 0;
        else
            REG(GET_R1) = 0xFFFFFFFF;
    }
    else
    {
        SET_C((op1 >> (op2 - 1)) & 0x1);
        if(op1 > 0x7FFFFFFF) // neg number => keep neg sign
        {
            for (int j = 0; j < op2; j++) {
                    op1 = (op1 >> 1) | 0x80000000;
            }
            REG(GET_R1) = op1;
        }
        else
            REG(GET_R1) = op1 >> op2;
    }
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_bic(uint32_t instruction) /* format 4 */
{
    DEBUG("BIC %s, %s\n",RR(GET_R1),RR(GET_R2));
    REG(GET_R1)=GET_REG(GET_R1) & (~GET_REG(GET_R2));
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_cmn(uint32_t instruction) /* format 4 */
{
    uint32_t op1 = GET_REG(GET_R1);
    uint32_t op2 = GET_REG(GET_R2);
    uint32_t alu_out = op1 + op2;
    DEBUG("CMN %s, %s\n",RR(GET_R1),RR(GET_R2));
    alu_out=op1 + op2; // RnVal - (-shifter_operand)
    ARM_NegZero(alu_out);
    ARM_SubCarry(op1,-op2, alu_out);
    ARM_SubOverflow(op1,-op2, alu_out);
}

#define DO_THUMB_CMP(OP1,OP2) {                      \
    uint32_t alu_out=OP1 - OP2;                 \
    ARM_NegZero(alu_out);                            \
    ARM_SubCarry(OP1,OP2, alu_out);    \
    ARM_SubOverflow(OP1,OP2, alu_out); \
}

void thumb_cmp_Reg_imm(uint32_t instruction) /* format 2 */
{
    uint32_t op1 = GET_REG(((instruction>>8)&0x7));
    uint32_t op2 = GET_REG(instruction&0xFF);
    DEBUG("CMP %s 0x%x\n",RR(((instruction>>8)&0x7)),op2);
    DO_THUMB_CMP(op1,op2)
}

void thumb_cmp_2Reg(uint32_t instruction) /* format 4 */
{
    uint32_t op1 = GET_REG(GET_R1);
    uint32_t op2 = GET_REG(GET_R2);
    DEBUG("CMP %s %s\n",RR(GET_R1),RR(GET_R2));
    DO_THUMB_CMP(op1,op2)
}

void thumb_cmp_long_R(uint32_t instruction) /* format 7 */
{
    uint32_t op1 = GET_REG(GET_LR1);
    uint32_t op2 = GET_REG(GET_LR2);
    DEBUG("CMP %s %s\n",RR(GET_LR1),RR(GET_LR2));
    DO_THUMB_CMP(op1,op2)
}


void thumb_eor(uint32_t instruction) /* format 4 */
{
    DEBUG("EOR %s, %s\n",RR(GET_R1),RR(GET_R2));
    REG(GET_R1)=GET_REG(GET_R1) ^ GET_REG(GET_R2);
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_lsl_2Reg(uint32_t instruction) /* format 4 */
{
    uint32_t op1 = GET_REG(GET_R1);
    uint32_t op2 = GET_REG(GET_R2) & 0xFF;
    
    DEBUG("LSL %s, %s\n",RR(GET_R1),RR(GET_R2));
    if((op2) == 0)
    {
        /* nothing to do */
    }
    else if((op2) < 32)
    {
        SET_C((op1 >> (32 - op2)) & 0x1);
        REG(GET_R1)=op1 << op2;
    }
    else if(op2 == 32)
    {
        SET_C(op1 & 0x1);
        REG(GET_R1) = 0;
    }
    else
    {
        SET_C(false);
        REG(GET_R1) = 0;
    }
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_lsl_2Reg_imm(uint32_t instruction) /* format 3 */
{
    uint32_t op2 = (instruction >> 6) & 0x1F;
    DEBUG("LSL %s, %s, 0x%x",RR(GET_R1),RR(GET_R2),op2);
    if(op2 == 0)
    {
       REG(GET_R1)=GET_REG(GET_R2);
    }
    else
    {
       SET_C(((GET_REG(GET_R2)) >> (32 - op2)) & 0x1);
       REG(GET_R1)=(GET_REG(GET_R2)) << op2;
    }
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_lsr_2Reg(uint32_t instruction) /* format 4 */
{
    uint32_t op1 = GET_REG(GET_R1);
    uint32_t op2 = GET_REG(GET_R2) & 0xFF;
    
    DEBUG("LSR %s, %s\n",RR(GET_R1),RR(GET_R2));
    if((op2) == 0)
    {
        /* nothing to do */
    }
    else if(op2 < 32)
    {
        SET_C((op1 >> (op2-1)) & 0x1);
        REG(GET_R1)=op1 >> op2;
    }
    else if(op2 == 32)
    {
        SET_C((GET_REG(GET_R1) >> 31) & 0x1);
        REG(GET_R1) = 0;
    }
    else if(op2 > 32)
    {
        SET_C(false);
        REG(GET_R1) = 0;
    }
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_lsr_2Reg_imm(uint32_t instruction) /* format 3 */
{
    uint32_t op2 = (instruction >> 6) & 0x1F;
    DEBUG("LSR %s, %s, 0x%x",RR(GET_R1),RR(GET_R2),op2);
    if(op2 == 0)
    {
       SET_C(((GET_REG(GET_R2)) >> 31) & 0x1);
       REG(GET_R1)=0;
    }
    else
    {
       SET_C(((GET_REG(GET_R2)) >> (op2-1)) & 0x1);
       REG(GET_R1)=(GET_REG(GET_R2)) >> op2;
    }
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_mov_Reg_imm(uint32_t instruction) /* format 2 */
{
    uint32_t op2 = GET_REG(instruction&0xFF);
    DEBUG("MOV %s 0x%x\n",RR(((instruction>>8)&0x7)),op2);
    REG(((instruction>>8)&0x7)) = op2;
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_mov_2Reg(uint32_t instruction) /* format 4 */
{
    DEBUG("MOV %s %s\n",RR(GET_R1),RR(GET_R2));
    REG(GET_R1) = GET_REG(GET_R2);
    ARM_NegZero(GET_REG(GET_R1));
    SET_C(false);
    SET_V(false);
}

void thumb_mov_long_R(uint32_t instruction) /* format 7 */
{
    uint32_t op1 = GET_REG(GET_LR1);
    uint32_t op2 = GET_REG(GET_LR2);
    DEBUG("MOV %s %s\n",RR(GET_LR1),RR(GET_LR2));
    if(GET_LR2 == R_PC)
         REG(GET_R1) = GET_REG(GET_LR2) +1;
    else
         REG(GET_R1) = GET_REG(GET_LR2);
}

void thumb_mul(uint32_t instruction) /* format 4 */
{
    DEBUG("MUL %s, %s\n",RR(GET_R1),RR(GET_R2));
    REG(GET_R1)=GET_REG(GET_R1) * GET_REG(GET_R2);
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_mvn(uint32_t instruction) /* format 4 */
{
    DEBUG("MVN %s, %s\n",RR(GET_R1),RR(GET_R2));
    REG(GET_R1)=~GET_REG(GET_R2);
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_neg(uint32_t instruction) /* format 4 */
{
    uint32_t op2 = GET_REG(GET_R2);
    DEBUG("NEG %s, %s\n",RR(GET_R1),RR(GET_R2));
    REG(GET_R1)=0-op2;
    ARM_NegZero(GET_REG(GET_R1));
    ARM_SubCarry(0,op2, 0-op2);
    ARM_SubOverflow(0,op2, 0-op2);
}

void thumb_or(uint32_t instruction) /* format 4 */
{
    DEBUG("OR %s, %s\n",RR(GET_R1),RR(GET_R2));
    REG(GET_R1)=GET_REG(GET_R1) | GET_REG(GET_R2);
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_ror(uint32_t instruction) /* format 4 */
{
    uint32_t op1 = GET_REG(GET_R1);
    uint32_t op2 = GET_REG(GET_R2) & 0xFF;
    DEBUG("ROR %s %s\n",RR(GET_R1),RR(GET_R2));
    if(op2 == 0)
    {
        /* nothing to do */
    }
    else if((op2 & 0xF) == 0)
    {
        SET_C((op1 >> 31) & 0x1);
    }
    else
    {
	op2 &=0xF;
       SET_C((op1 >> (op2-1)) & 0x1);
       REG(GET_R1) = (op1 << (32-op2)) | (op1 >> op2);
    }
    ARM_NegZero(GET_REG(GET_R1));
}

void thumb_sbc(uint32_t instruction) /* format 4 */
{
    uint32_t op1 = GET_REG(GET_R1);
    uint32_t op2 = GET_REG(GET_R2);

    DEBUG("SBC %s, %s\n",RR(GET_R1),RR(GET_R2));
    int addOn = ~(C_FLAG?1:0);
    REG(GET_R1) = op1 - op2 - addOn;
    ARM_NegZero(GET_REG(GET_R1));
    ARM_SubCOverflow(op1,op2, addOn);
    ARM_SubCCarry(op1,op2, addOn);
}

#define DO_SUB(REG_N,OP1,OP2) {             \
    REG(REG_N) = OP1 - OP2;                 \
    ARM_NegZero(GET_REG(REG_N));            \
    ARM_SubCarry(OP1,OP2, OP1 - OP2);     \
    ARM_SubOverflow(OP1,OP2, OP1 - OP2);  \
}

void thumb_sub_3Reg(uint32_t instruction) /* format 0 */
{
    uint32_t op1 = GET_REG(GET_R2);
    uint32_t op2 = GET_REG(GET_R3);
    DEBUG("SUB %s, %s, %s\n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    DO_SUB(GET_R1,op1,op2)
}

void thumb_sub_2Reg_imm(uint32_t instruction) /* format 1 */
{
    uint32_t op1 = GET_REG(GET_R2);
    uint32_t op2 = GET_R3;
    DEBUG("SUB %s, %s, 0x%x\n",RR(GET_R1),RR(GET_R2),op2);
    DO_SUB(GET_R1,op1,op2)
}

void thumb_sub_Reg_imm(uint32_t instruction) /* format 2 */
{
    
    uint32_t op1 = GET_REG(((instruction>>8)&0x7));
    uint32_t op2 = (instruction&0xFF);
    DEBUG("SUB %s, 0x%x\n",RR(((instruction>>8)&0x7)),op2);
    DO_SUB(((instruction>>8)&0x7),op1,op2)
}

void thumb_sub_SP_imm(uint32_t instruction) /* format 6 */
{
    uint32_t op2 = (instruction&0x7F)<<2;
    DEBUG("SUB SP, 0x%x\n",op2);
    REG(R_SP) = GET_REG(R_SP) - op2;
}

void thumb_tst(uint32_t instruction) /* format 4 */
{
    DEBUG("TST %s, %s\n",RR(GET_R1),RR(GET_R2));
    ARM_NegZero((GET_REG(GET_R1) & GET_REG(GET_R2)));
}


