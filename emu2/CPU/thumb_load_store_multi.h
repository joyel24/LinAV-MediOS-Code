
void thumb_ldmia(uint32_t instruction)
{
    int register_list = instruction & 0xFF;
    int Rn = (instruction >> 8) & 0x7;
    uint32_t address = GET_REG(Rn);
    bool is_present = false;
    DEBUG("LDMIA %s!, ",RR(Rn));
    for(int k =0; k < 8; k++)
    {
        if((register_list & 0x1) == 0x1)
        {
            REG(k) = mem->read(address,4);
            address += 4;
            DEBUG("%s ",RR(k));
            if(k==Rn)
                is_present = true;
        }
        register_list = register_list >> 1;
    }
    if(!is_present)
        REG(Rn)=address;
}

void thumb_stmia(uint32_t instruction)
{
    int register_list = instruction & 0xFF;
    int Rn = (instruction >> 8) & 0x7;
    uint32_t address = GET_REG(Rn);
    DEBUG("STMIA %s!, ",RR(Rn));
    for(int k =0; k < 8; k++)
    {
        if((register_list & 0x1) == 0x1)
        {
            mem->write(address,GET_REG(k),4);
            address +=4;
            DEBUG("%s ",RR(k));
        }
        register_list = register_list >> 1;
    }
    REG(Rn)=address;
}

void thumb_pop(uint32_t instruction)
{
    DEBUG("POP ");
    uint32_t address = GET_REG(R_SP);
    int register_list = instruction & 0xFF;
    
    for(int k=0; k<8;k++)
    {
        if((register_list & 0x1) == 0x1)
        {
            REG(k) = mem->read(address,4);
            address += 4;
            DEBUG("%s ",RR(k));
        }
        register_list = register_list >> 1;
    }                    
    if((instruction >> 8)  & 0x1)
    {
        int value=mem->read(address,4);
        REG(R_PC) = value & 0xFFFFFFFE;
        address +=4;
        DEBUG("PC ");
        SET_T((value & 0x1));
    }
    REG(R_SP)=address;
    DEBUG("\n");
}

void thumb_push(uint32_t instruction)
{
    DEBUG("PUSH ");
    int bit_R = (instruction >> 8)  & 0x1;
    int register_list = instruction & 0xFF;
    int nbSet=0;
    int tmpList=register_list;
    uint32_t address;
    uint32_t start_address;
    for(int k=0;k<8;k++) // calculate the number of set bits;
    {
        if((tmpList & 0x1) == 0x1)
            nbSet++;
        tmpList = tmpList >> 1;
    }
    if(bit_R)
            nbSet++;
    start_address=GET_REG(R_SP) - 4 * nbSet;
    address = start_address;
    for(int k=0;k<8;k++)
    {
        if((register_list & 0x1) == 0x1)
        {
            mem->write(address,GET_REG(k),4);
            address +=4;
            DEBUG("%s ",RR(k));
        }
        register_list = register_list >> 1;
    }
    if(bit_R)
    {
            mem->write(address,GET_REG(R_LR),4);
            address +=4;
            DEBUG("LR ");
    }
    REG(R_SP)=start_address;
    DEBUG("\n");
}
