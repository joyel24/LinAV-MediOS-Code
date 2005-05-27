void Cpu::ARM_NegZero(uint32_t result)
{
    if(result == 0)
    {
        SET_Z(true);
    }
    else
    {
        SET_Z(false);
    }
    
    if((result>>31)&0x1)
    {
        SET_N(true);
    }
    else
    {
        SET_N(false);
    }

  /*  if (result < 0) {
        SET_N(true);
        SET_Z(false);
    } else if (result == 0) {
        SET_N(false);
        SET_Z(true);
    } else {
        SET_N(false);
        SET_Z(false);
    }*/
}


void Cpu::ARM_AddCarry(uint32_t a, uint32_t b, uint32_t result)
{
    bool tmpflag = false;
    if (((a < 0) && (b < 0)) ||
        ((a < 0) && (result >= 0)) ||
        ((b < 0) && (result >= 0))) {
        tmpflag = true;
    }
    SET_C(tmpflag);
}


void Cpu::ARM_AddOverflow(uint32_t a, uint32_t b, uint32_t result)
{
    bool tmpflag = false;
    if ((a < 0) && (b < 0) && (result >= 0) ||
        (a >= 0) && (b >= 0) && (result < 0)) {
        tmpflag  = true;
    }
    SET_V(tmpflag);
}


void Cpu::ARM_SubCarry(uint32_t a, uint32_t b, uint32_t result)
{
    bool tmpflag = false;
    if (((a < 0) && (b >= 0)) ||
        ((a < 0) && (result >= 0)) ||
        ((b >= 0) && (result >= 0))) {
        tmpflag = true;
    }
    SET_C(tmpflag);
}


void Cpu::ARM_SubOverflow(uint32_t a, uint32_t b, uint32_t result)
{
    bool tmpflag = false;
    if ((a < 0) && (b >= 0) && (result >= 0) ||
        (a >= 0) && (b < 0) && (result < 0)) {
        tmpflag = true;
    }
    SET_V(tmpflag);
}

/////////////// with carry version

void Cpu::ARM_AddCCarry(uint32_t a, uint32_t b, uint32_t c)
{
    ARM_AddCarry(a,b,a+b);
    if(!C_FLAG  && c !=0)
        ARM_AddCarry(a+b,c,a+b+c);
#warning add carry test
}

void Cpu::ARM_AddCOverflow(uint32_t a, uint32_t b, uint32_t c)
{
    ARM_AddOverflow(a,b,a+b);
    if(!V_FLAG  && c !=0)
            Cpu::ARM_AddOverflow(a+b,c,a+b+c);
#warning add overflow test
}

void Cpu::ARM_SubCCarry(uint32_t a, uint32_t b,uint32_t c)
{
    ARM_SubCarry(a,b,a-b);
    if(!V_FLAG  && c !=0)
            ARM_SubCarry(a-b,c,a-b-c);
#warning sub carry test
}

void Cpu::ARM_SubCOverflow(uint32_t a, uint32_t b,uint32_t c)
{
    ARM_SubOverflow(a,b,a-b);
    if(!V_FLAG  && c !=0)
            ARM_SubOverflow(a-b,c,a-b-c);
#warning sub overflow test
}
