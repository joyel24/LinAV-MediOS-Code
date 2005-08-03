
#define DO_LDR(OP1,OP2) {                   \
    uint32_t address = OP1 + OP2;           \
    if((address & 0x3) == 0)                \
        REG(GET_R1) = mem->read(address,4); \
    else                                    \
    {                                       \
        INT_DEBUG_HEAD_THUMB                \
        printf("Unpredictable LDR\n");      \
        exit(0);                            \
    }                                       \
}

void thumb_ldr_2Reg_imm(uint32_t instruction)
{
    DEBUG("LDR %s, [%s, 0x%x] \n",RR(GET_R1),RR(GET_R2),(instruction>>4)&0x7C);
    DO_LDR((GET_REG(GET_R2)),((instruction>>4)&0x7C))
}

void thumb_ldr_3Reg(uint32_t instruction)
{
    DEBUG("LDR %s, [%s, %s] \n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    DO_LDR((GET_REG(GET_R2)),(GET_REG(GET_R3)))
}

void thumb_ldr_Reg_PC_imm(uint32_t instruction)
{
    DEBUG("LDR %s, [PC, 0x%x] \n",RR(GET_R1),instruction&0xFF);
    REG(((instruction>>8)&0x7)) = mem->read((GET_REG(R_PC) & 0xFFFFFFFC)+(instruction&0xFF),4);
}

void thumb_ldr_Reg_SP_imm(uint32_t instruction)
{
    DEBUG("LDR %s, [SP, 0x%x] \n",RR(GET_R1),instruction&0xFF);
    DO_LDR((GET_REG(R_SP)),(instruction&0xFF))
}

void thumb_ldrb_2Reg_imm(uint32_t instruction)
{
    DEBUG("LDRB %s, [%s, 0x%x] \n",RR(GET_R1),RR(GET_R2),(instruction>>6)&0x1F);
    REG(GET_R1) = mem->read(GET_REG(GET_R2)+((instruction>>6)&0x1F),1);
}

void thumb_ldrb_3Reg(uint32_t instruction)
{
    DEBUG("LDRB %s, [%s, %s] \n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    REG(GET_R1) = mem->read(GET_REG(GET_R2)+GET_REG(GET_R3),1);
}

#define DO_LDRH(OP1,OP2) {                   \
    uint32_t address = OP1 + OP2;           \
    if((address & 0x1) == 0)                \
        REG(GET_R1) = mem->read(address,2); \
    else                                    \
    {                                       \
        INT_DEBUG_HEAD_THUMB                \
        printf("Unpredictable LDRH\n");      \
        exit(0);                            \
    }                                       \
}

void thumb_ldrh_2Reg_imm(uint32_t instruction)
{
    DEBUG("LDRH %s, [%s, 0x%x] \n",RR(GET_R1),RR(GET_R2),(instruction>>5)&0x1E);
    DO_LDRH((GET_REG(GET_R2)),((instruction>>5)&0x1E))
}

void thumb_ldrh_3Reg(uint32_t instruction)
{
    DEBUG("LDRH %s, [%s, %s] \n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    DO_LDRH((GET_REG(GET_R2)),(GET_REG(GET_R3)))
}

void thumb_ldrsb_3Reg(uint32_t instruction)
{
    DEBUG("LDRSB %s, [%s, %s] \n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    REG(GET_R1) = signExtend1(mem->read(GET_REG(GET_R2)+GET_REG(GET_R3),1));
}

void thumb_ldrsh_3Reg(uint32_t instruction)
{
    DEBUG("LDRSH %s, [%s, %s] \n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    uint32_t address = GET_REG(GET_R2) + GET_REG(GET_R3);
    if((address & 0x1) == 0)
        REG(GET_R1) = signExtend2(mem->read(address,2));
    else
    {
        INT_DEBUG_HEAD_THUMB
        printf("Unpredictable LDRSH\n");
        exit(0);
    }
}

#define DO_STR(OP1,OP2) {                      \
    uint32_t address = OP1 + OP2;              \
    if((address & 0x3) == 0)                   \
        mem->write(address,GET_REG(GET_R1),4); \
    else                                       \
    {                                          \
        INT_DEBUG_HEAD_THUMB                   \
        printf("Unpredictable STR\n");         \
        exit(0);                               \
    }                                          \
}

void thumb_str_2Reg_imm(uint32_t instruction)
{
    DEBUG("STR %s, [%s, 0x%x] \n",RR(GET_R1),RR(GET_R2),(instruction>>4)&0x7C);
    DO_STR((GET_REG(GET_R2)),((instruction>>4)&0x7C))
}

void thumb_str_3Reg(uint32_t instruction)
{
    DEBUG("STR %s, [%s, %s] \n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    DO_STR((GET_REG(GET_R2)),(GET_REG(GET_R3)))
}

void thumb_str_Reg_SP_imm(uint32_t instruction)
{
    DEBUG("STR %s, [SP, 0x%x] \n",RR(GET_R1),instruction&0xFF);
    DO_STR((GET_REG(R_SP)),(instruction&0xFF))
}

void thumb_strb_2Reg_imm(uint32_t instruction)
{
    DEBUG("STRB %s, [%s, 0x%x] \n",RR(GET_R1),RR(GET_R2),(instruction>>6)&0x1F);
    mem->write(GET_REG(GET_R2)+((instruction>>6)&0x1F),GET_REG(GET_R1),1);
}

void thumb_strb_3Reg(uint32_t instruction)
{
    DEBUG("STRB %s, [%s, %s] \n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    mem->write(GET_REG(GET_R2)+GET_REG(GET_R3),GET_REG(GET_R1),1);
}

#define DO_STRH(OP1,OP2) {                     \
    uint32_t address = OP1 + OP2;              \
    if((address & 0x1) == 0)                   \
        mem->write(address,GET_REG(GET_R1),2); \
    else                                       \
    {                                          \
        INT_DEBUG_HEAD_THUMB                   \
        printf("Unpredictable STRH\n");        \
        exit(0);                               \
    }                                          \
}

void thumb_strh_2Reg_imm(uint32_t instruction)
{
    DEBUG("STRH %s, [%s, 0x%x] \n",RR(GET_R1),RR(GET_R2),(instruction>>5)&0x1E);
    DO_STRH((GET_REG(GET_R2)),((instruction>>5)&0x1E))
}

void thumb_strh_3Reg(uint32_t instruction)
{
    DEBUG("STRH %s, [%s, %s] \n",RR(GET_R1),RR(GET_R2),RR(GET_R3));
    DO_STRH((GET_REG(GET_R2)),(GET_REG(GET_R3)))
}

