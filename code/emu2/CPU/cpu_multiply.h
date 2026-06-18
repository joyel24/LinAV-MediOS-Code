void arm_Mul(int condCode,uint32_t instruction)
{
    if(!checkCondition(condCode))
    {
        DEBUG("CC not met\n");
        return;
    }
    
    int Rd,Rn,Rs,RdHi,RdLo,Rm;
    uint32_t val,valHi,valLo;
    bool bit_U,bit_A,bit_S,longM;

    Rm= instruction & 0xF;
    Rs= (instruction>>8) & 0xF;
    longM=(instruction>>23) & 0x1;
    bit_S=(instruction>>20) & 0x1;
    bit_A=(instruction>>21) & 0x1;

    if(!longM)                                 // multiply normal
    {
        Rd= (instruction>>16) & 0xF;
        Rn= (instruction>>12) & 0xF;
        if(bit_A)
        {
            REG(Rd) = (uint32_t)((((uint64_t)GET_REG(Rm) * (uint64_t)GET_REG(Rs))+(uint64_t)GET_REG(Rn)) & 0x00000000FFFFFFFF);
            DEBUG("MLA %s, %s, %s, %s\n",RR(Rd),RR(Rn),RR(Rm),RR(Rs));
        }
        else
        {
            REG(Rd) = (uint32_t)(((uint64_t)GET_REG(Rm) * (uint64_t)GET_REG(Rs)) & 0x00000000FFFFFFFF);
            DEBUG("MUL %s, %s, %s\n",RR(Rd),RR(Rm),RR(Rs));
        }
        
        if(bit_S)
        {
            ARM_NegZero(GET_REG(Rd));
        }
    }
    else                                       // multiply long
    {
        bit_U=(instruction>>22) & 0x1;
        RdHi=(instruction>>16) &0xF;
        RdLo=(instruction>>12) &0xF;

        uint64_t value;

        DEBUG("%sM%sL%s ",bit_U?"S":"U",bit_A?"LA":"UL",bit_S?"S":"");

        if (bit_U)
        {
            value = (uint64_t)GET_REG(Rm) * (uint64_t)GET_REG(Rs);

            if(bit_A) // accumulate
            {
                value += (((uint64_t)REG(RdHi))<<32) | ((uint64_t)REG(RdLo));
            }
        }
        else
        {
            value = (uint64_t)((sint64_t)GET_REG(Rm) * (sint64_t)GET_REG(Rs));

            if(bit_A) // accumulate
            {
                value = (uint64_t)(((sint64_t)value) + (sint64_t)(((uint64_t)REG(RdHi))<<32) | ((uint64_t)REG(RdLo)));
            }
        }

        REG(RdHi) = (uint32_t)(value >> 32);
        REG(RdLo) = (uint32_t)(value & 0x00000000FFFFFFFF);

        if(bit_S)
        {
                SET_N(((valHi >> 31) & 0x1) == 1);
                SET_Z((valHi == 0 && valLo == 0));
        }
    }
}
