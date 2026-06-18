void arm_LoadStoreMulti(uint32_t instruction)
{
    bool bit_P = ((instruction >> 24) & 0x1);
    bool bit_U = ((instruction >> 23) & 0x1);
    bool bit_S = ((instruction >> 22) & 0x1);
    bool bit_W = ((instruction >> 21) & 0x1);
    bool bit_L = ((instruction >> 20) & 0x1);

    int Rn = (instruction >> 16) & 0xF;
    int register_list = instruction & 0xFFFF;
    int tmpList=register_list;
    uint32_t start_address=0,end_address=0;
    int nbSet=0;
    
    char * debugShifter;
    char debugShifter2[50];
    
    for(int k=0;k<16;k++)                         // calculate the number of set bits;
    {
        if((tmpList & 0x1))
            nbSet++;
        tmpList = tmpList >> 1;
    }

    if(!bit_P && bit_U)                           // increment after
    {
        if(Rn == 13)
            if(bit_L)
                debugShifter="FD ";
            else
                debugShifter="EA ";
        else
            debugShifter="IA ";
        start_address=GET_REG(Rn);
        end_address=start_address+(nbSet*4)-4;
        if(bit_W)
            REG(Rn)=GET_REG(Rn)+nbSet*4;
    }
    else if(bit_P && bit_U)                        // increment before
    {
        if(Rn == 13)
            if(bit_L)
                debugShifter="ED ";
            else
                debugShifter="FA ";
        else
            debugShifter="IB ";
        start_address=GET_REG(Rn)+4;
        end_address=start_address+(nbSet*4);
        if(bit_W)
            REG(Rn)=GET_REG(Rn)+nbSet*4;
    }
    else if(!bit_P && !bit_U)                      // decrement after
    {
            if(Rn == 13)
                if(bit_L)
                    debugShifter="FA ";
                else
                    debugShifter="ED ";
            else
                debugShifter="DA ";
            start_address=GET_REG(Rn)-(nbSet*4)+4;
            end_address=GET_REG(Rn);
            if(bit_W)
                REG(Rn)=GET_REG(Rn)-nbSet*4;
    }
    else                                          // decrement before
    {
        if(Rn == 13)
            if(bit_L)
                debugShifter="EA ";
            else
                debugShifter="FD ";
        else
            debugShifter="DB ";
        start_address=GET_REG(Rn)-(nbSet*4);
        end_address=GET_REG(Rn)-4;
        if(bit_W)
            REG(Rn)=GET_REG(Rn)-nbSet*4;
    }

    MKDEBUG(debugShifter2,"%s %s%s",debugShifter,RR(Rn),bit_W?"!":" ");
    
    if(bit_L)                                          // Load
    {
        DEBUG("LDM%s ", debugShifter2);

        bool has_PC = (register_list>>15)&0x1==0x1;

        uint32_t address = start_address;
        for(int k=0;k<15;k++)
        {
            if(register_list & 0x1)
            {
                if(bit_S && !has_PC)
                    *mode_regs[M_USER][k]=mem->read(address,4);
                else
                    REG(k)=mem->read(address,4);
                address+=4;
                DEBUG("%s ",RR(k));
            }
            register_list=register_list >> 1;
        }

        if(register_list & 0x1) // test for PC (R15)
        {
            DEBUG("PC\n");
            uint32_t val;
            if(bit_S)
            {
                int old_mode=MODE;
                if(old_mode == M_USER || old_mode == M_SYS)  
                {              
                    //printf("Unpredictable, wrong mode\n");
                    //exit(0);
                }
                else
                {
                    REG(R_CPSR)=REG(R_SPSR);
                    CHK_T_FLAG_FCT
                    CHK_IRQ_FCT
                    CHK_FIQ_FCT
                }
                    
                if(old_mode != MODE)
                {
                    //printf("Mode has changed from %s to %s\n",mode_str[old_mode],mode_str[MODE]);
                    current_reg = mode_regs[MODE];
                }
                    
            }
            
            val=mem->read(address,4);
            
            REG(R_PC)=val & 0xFFFFFFFE;
            
            /*if(val & 0x1)
            {
                SET_FLAG(T_MASK);
                CHK_T_FLAG_FCT
            }
            else
            {
                CLR_FLAG(T_MASK);
                CHK_T_FLAG_FCT
            }*/
            address+=4;
        }
        else
        {
            DEBUG("\n");
        }
    }
    else                                          // Store
    {
        DEBUG("STM%s ", debugShifter2);
        uint32_t address = start_address;
        for(int k=0;k<16;k++)
        {
            if(register_list & 0x1)
            {
                mem->write(address,GET_REG(k),4);
                address+=4;
                DEBUG("%s ",RR(k));
            }
            register_list=register_list >> 1;
        }
        DEBUG("\n");
    }
}
