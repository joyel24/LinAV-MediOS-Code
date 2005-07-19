void thumb_load_store_multi(int format,int opcode,uint32_t instruction)
{
    int register_list = instruction & 0xFF;
    int Rn;
    bool bit_R ;
    uint32_t address;
    int nb_set=0;
    bool is_present = false;
    
    switch(format)
    {
        case 0x0:
            Rn = (instruction >> 8) & 0x7;
            address = GET_REG(Rn);
            if(opcode)
            {
                DEBUG("LDMIA %s!, ",RR(Rn));
                for(int k =0; k < 8; k++)
                {
                    if((register_list & 0x1) == 0x1)
                    {
                        REG(k) = mem->read(address,4);
                        address +=4;
                        DEBUG("%s ",RR(k));
                        nb_set++;
                        if(k==Rn)
                            is_present = true;
                    }
                    register_list = register_list >> 1;
                }
                if(!is_present)
                    REG(Rn)=GET_REG(Rn)+4*nb_set;
            }
            else
            {
                DEBUG("STMIA %s!, ",RR(Rn));
                for(int k =0; k < 8; k++)
                {
                    if((register_list & 0x1) == 0x1)
                    {
                        mem->write(address,GET_REG(k),4);
                        address +=4;
                        DEBUG("%s ",RR(k));
                        nb_set++;
                    }
                    register_list = register_list >> 1;
                }
                REG(Rn)=GET_REG(Rn)+4*nb_set;
            }
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
