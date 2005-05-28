void Cpu::arm_Mul(int condCode,uint32_t instruction)
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
        REG(Rd)=GET_REG(Rm) * GET_REG(Rs);
        
        DEBUG("MUL %s, %s, %s\n",RR(Rd),RR(Rm),RR(Rs));
               
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

        uint32_t valL;

        DEBUG("%sM%sL%s ",bit_U?"S":"U",bit_A?"AL":"UL",bit_S?"S":"");
        
        
/*
        valL=readReg(Rm) * readReg(Rs);
        valHi=(int)((valL>>32) & 0xFFFFFFFFL);
        valLo=(int)(valL & 0xFFFFFFFFL);

        if(bit_A) // accumulate
        {
                valHi += readReg(RdHi);
                boolean savC=flagC();	// saving C flag
                ARM_AddCarry(valLo, readReg(RdLo), valLo + readReg(RdLo)) ; // warning need to be changed
                if(flagC())
                        valHi++;
                valLo += readReg(RdLo);
                flagC(savC);
        }
        

        regs[RdHi][regMode(RdHi,mode())]=valHi;
        regs[RdLo][regMode(RdLo,mode())]=valLo;

        if(bit_S)
        {
                flagN(((valHi >> 31) & 0x1) == 1);
                flagZ((valHi == 0 && valLo == 0));
        }
*/
    }
}
