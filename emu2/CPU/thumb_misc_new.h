void thumb_swi(uint32_t instruction)
{
    DEBUG("SWI :%08x\n",instruction & 0x000000FF);
    *mode_regs[M_SVC][R_LR]=PC_REAL;
    *mode_regs[M_SVC][R_SPSR]=REG(R_CPSR);
    SET_MODE(M_SVC);
    CLR_FLAG(T_MASK);
    SET_FLAG(IRQ_MASK);
    REG(R_PC)=0x8;
}

void thumb_bkpt(uint32_t instruction)
{
    INT_DEBUG_HEAD_THUMB
    printf("BKT undef for ARMv4\n");
    exit(0);
}

void thumb_undef(uint32_t instruction)
{
    INT_DEBUG_HEAD_THUMB
    printf("undefined instruction (3) %x\n",instruction);
    exit(0);
}

void thumb_B_cond(uint32_t instruction)
{
    int cond = (instruction>>8)&0xF;
    
    switch(cond)
    {
        case 0xF: /*SWI*/
            DEBUG("SWI :%08x\n",instruction & 0x000000FF);
            *mode_regs[M_SVC][R_LR]=PC_REAL;
            *mode_regs[M_SVC][R_SPSR]=REG(R_CPSR);
            SET_MODE(M_SVC);
            CLR_FLAG(T_MASK);
            SET_FLAG(IRQ_MASK);
            REG(R_PC)=0x8;
            break;
        case 0xE: /* UNDEF */
            INT_DEBUG_HEAD_THUMB
            printf("undefined instruction (3) %x\n",instruction);
            exit(0);
            break;
        default:
            DEBUG("B<%s> ",cond_str[cond]);    
            if(checkCondition(cond))
            {
                REG(R_PC) = GET_REG(R_PC) + (signExtend1((instruction&0xFF))<<1);
                DEBUG("=> 0x%08x\n",PC_REAL);                            
            }
            else
                DEBUG("CC not met\n");
    }
}

void thumb_B(uint32_t instruction)
{
    REG(R_PC)=GET_REG(R_PC) + signExtend11(instruction & 0x7FF)<<1;
    DEBUG("B => 0x%08x\n",PC_REAL);   
}

void thumb_BL_S1(uint32_t instruction)
{
    REG(R_LR)=GET_REG(R_PC)+signExtend11(instruction & 0x7FF)<<12;
    DEBUG("BL stp1\n");   
}

void thumb_BL_S2(uint32_t instruction)
{
    uint32_t offset=GET_REG(R_LR)+((instruction & 0x7FF)<<1);
    REG(R_LR) = PC_REAL | 1 ;
    REG(R_PC) = offset;
    DEBUG("BL => 0x%08x\n",offset);   
}

void thumb_BX(uint32_t instruction)
{
    int Rm = (instruction>>3)&0xF;
    uint32_t dest = GET_REG(Rm) & 0xFFFFFFFE;
    SET_T(GET_REG(Rm) & 0x1);
    REG(R_PC) = dest;
    DEBUG("BX %s => 0x%08x\n",GET_REG(Rm) & 0x1?"THUMB":"ARM",dest);
}
