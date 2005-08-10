#define isNeg(VAL)   (((VAL)>>31)&0x1)

void ARM_NegZero(uint32_t result)
{
    if(result == 0)
    {
        SET_Z(true);
    }
    else
    {
        SET_Z(false);
    }
    
    if(isNeg(result))
    {
        SET_N(true);
    }
    else
    {
        SET_N(false);
    }
}


void ARM_AddCarry(uint32_t a, uint32_t b, uint32_t result)
{
    bool tmpflag = false;
    if ((isNeg(a) && isNeg(b)) ||
        (isNeg(a) && !isNeg(result)) ||
        (isNeg(b) && !isNeg(result))) {
        tmpflag = true;
    }
    SET_C(tmpflag);
}


void ARM_AddOverflow(uint32_t a, uint32_t b, uint32_t result)
{
    bool tmpflag = false;
    if (isNeg(a) && isNeg(b) && !isNeg(result) ||
        !isNeg(a) && !isNeg(b) && isNeg(result)) {
        tmpflag  = true;
    }
    SET_V(tmpflag);
}


void ARM_SubCarry(uint32_t a, uint32_t b, uint32_t result)
{
    bool tmpflag = false;
    if ((isNeg(a) && !isNeg(b)) ||
        (isNeg(a) && !isNeg(result)) ||
        (!isNeg(b) && !isNeg(result))) {
        tmpflag = true;
    }
    SET_C(tmpflag);
}


void ARM_SubOverflow(uint32_t a, uint32_t b, uint32_t result)
{
    bool tmpflag = false;
    if (isNeg(a) && !isNeg(b) && !isNeg(result) ||
        !isNeg(a) && isNeg(b) && isNeg(result)) {
        tmpflag = true;
    }
    SET_V(tmpflag);
}

/////////////// with carry version

void ARM_AddCCarry(uint32_t a, uint32_t b, uint32_t c)
{
    ARM_AddCarry(a,b,a+b);
    if(!C_FLAG  && c !=0)
        ARM_AddCarry(a+b,c,a+b+c);
#warning add carry test
}

void ARM_AddCOverflow(uint32_t a, uint32_t b, uint32_t c)
{
    ARM_AddOverflow(a,b,a+b);
    if(!V_FLAG  && c !=0)
            ARM_AddOverflow(a+b,c,a+b+c);
#warning add overflow test
}

void ARM_SubCCarry(uint32_t a, uint32_t b,uint32_t c)
{
    ARM_SubCarry(a,b,a-b);
    if(!V_FLAG  && c !=0)
            ARM_SubCarry(a-b,c,a-b-c);
#warning sub carry test
}

void ARM_SubCOverflow(uint32_t a, uint32_t b,uint32_t c)
{
    ARM_SubOverflow(a,b,a-b);
    if(!V_FLAG  && c !=0)
            ARM_SubOverflow(a-b,c,a-b-c);
#warning sub overflow test
}

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
