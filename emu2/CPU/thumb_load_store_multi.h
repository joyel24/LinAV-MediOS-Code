void Cpu::thumb_load_store_multi(int format,int opcode,uint32_t instruction)
{
    int register_list = instruction & 0xFF;
    int Rn;
    bool bit_R ;
    uint32_t address;
    
    switch(format)
    {
        case 0x0:            
            DEBUG("%sMIA ",opcode?"LD":"ST");
            Rn = (instruction >> 8) & 0x7;
            address = GET_REG(Rn);
            for(int k =0; k < 8; k++)
            {
                if((register_list & 0x1) == 0x1)
                {
                        if(opcode)
                            REG(k) = mem->read(address,4);
                        else
                            mem->write(address,GET_REG(k),4);
                        address +=4;
                        DEBUG("% ",RR(k));
                }
                register_list = register_list >> 1;
            }
            REG(Rn)=address;
            DEBUG("\n");
            break;
        case 0x1:
            bit_R = (instruction >> 8)  & 0x1;
            
            if(opcode)                   // POP
            {
                    DEBUG("POP ");
                    address = GET_REG(R_SP);
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
                    if(bit_R)
                    {
                        int value=mem->read(address,4);
                        REG(R_PC) = value & 0xFFFFFFFE;
                        address +=4;
                        DEBUG("PC ");
                        SET_T(value & 0x1);
                    }
                    REG(R_SP)=address;
                    DEBUG("\n");
            }
            else                         // PUSH
            {
                    DEBUG("PUSH ");
                    int nbSet=0;
                    int tmpList=register_list;
                    for(int k=0;k<8;k++) // calculate the number of set bits;
                    {
                            if((tmpList & 0x1) == 0x1)
                                    nbSet++;
                            tmpList = tmpList >> 1;
                    }
                    if(bit_R)
                            nbSet++;
                    uint32_t start_address=GET_REG(R_SP) - 4 * nbSet;
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
            break;
    }
}
