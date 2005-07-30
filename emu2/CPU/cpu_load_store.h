void arm_LoadStore(int condCode,int instr_num,uint32_t instruction)
{
    int offset_12=0,shift=0,shift_imm=0,immedH=0,immedL=0;
    int Rm=0,Rn=0,Rd=0;
    
    Rn= ( instruction >> 16 ) & 0xf;
    Rd= ( instruction >> 12 ) & 0xf;
    
    bool bit_I = ((instruction >> 25 ) & 0x1 );
    bool bit_P = ((instruction >> 24 ) & 0x1 );
    bool bit_U = ((instruction >> 23 ) & 0x1 );
    bool bit_B = ((instruction >> 22 ) & 0x1 );
    bool bit_W = ((instruction >> 21 ) & 0x1 );
    bool bit_L = ((instruction >> 20 ) & 0x1 );
    bool bit_S=false;
    bool bit_H=false;
    
    char debugShifter[50];

    debugShifter[0]='X';
    debugShifter[1]='\0';
                    
    if(!bit_I)
    {
        if(instr_num == 0) // miscellaneous load store
        {
                if(bit_B) // immediate offset  // bit_B == special bit_I here
                {
                        bit_I = false;
                        immedL = instruction & 0xF;
                        immedH = (instruction >> 8) & 0xF;
                        bit_S = ((instruction >> 6) & 0x1) == 0x1;
                        bit_H = ((instruction >> 5) & 0x1) == 0x1;
                        offset_12 = (immedH << 4) | immedL;
                }
                else // register offset
                {
                        bit_I = true;
                        Rm = instruction & 0xF;
                        bit_S = ((instruction >> 6) & 0x1) == 0x1;
                        bit_H = ((instruction >> 5) & 0x1) == 0x1;
                }
        }
        else // immediate offset
                offset_12=instruction & 0xFFF;
    }
    else
    {
            Rm=instruction & 0xF;
            shift=(instruction >> 5) & 0x3;
            shift_imm=(instruction >> 7) & 0x1F;
    }

    uint32_t address=0;
    uint32_t offset=0;

    if(!bit_I) // immediate
    {
        offset=offset_12;
        MKDEBUG(debugShifter,"X 0x%x ",offset);
    }
    else // register
    {
        uint32_t RmVal=GET_REG(Rm);
        if(shift == 0 && shift_imm==0x0) // no shift
        {
                offset=RmVal;
                MKDEBUG(debugShifter,"X %s (0x%x) ",RR(Rm),offset);
        }
        else // with shift
        {
            switch(shift)
            {
                case 0x0: // LSL
                    offset= RmVal << shift_imm;
                    MKDEBUG(debugShifter,"X %s, LSL %d ",RR(Rm),shift_imm);
                    break;
                case 0x1: // LSR
                    if(shift_imm == 0)
                    {
                        offset= 0;
                        MKDEBUG(debugShifter,"X %s, LSR 32 ",RR(Rm));
                    }
                    else
                    {
                        offset= RmVal >> shift_imm;
                        MKDEBUG(debugShifter,"X %s, LSR %d ",RR(Rm),shift_imm);
                    }
                    break;
                case 0x2: // ASR
                    if(shift_imm == 0)
                    {
                        if(((RmVal >> 31) & 0x1) == 1)
                            offset= 0xFFFFFFFF;
                        else
                            offset= 0;
                        MKDEBUG(debugShifter,"X %s, ASR 32 ",RR(Rm)); 
                    }
                    else
                    {
                        if(RmVal > 0x7FFFFFFF) // neg number => keep neg sign
                        {
                            for (int j = 0; j < shift_imm; j++) {
                                RmVal = (RmVal >> 1) | 0x80000000;
                            }
                            offset = RmVal ;
                        }
                        else
                            offset = RmVal >> shift_imm;
                        MKDEBUG(debugShifter,"X %s, ASR %d ",RR(Rm),shift_imm);
                    }
                    break;
                case 0x3: // ROR or RRX
                    if(shift_imm == 0) 	// RRX
                    {
                        offset = RmVal >> 1;
                        if(C_FLAG)
                            offset=0x80000000 | offset;
                        MKDEBUG(debugShifter,"X %s, RRX ",RR(Rm));
                    }
                    else                                     // ROR
                    {
                        offset = (RmVal << (32-shift_imm) ) | (RmVal >> shift_imm);
                        MKDEBUG(debugShifter,"X %s, ROR %d ",RR(Rm),shift_imm);
                    }
                    break;
            }
        }
    }

    uint32_t RnVal=GET_REG(Rn);

    if(bit_P)
    {
        if(bit_U)
        {
            address = RnVal + offset;
            debugShifter[0] = '+';
        }
        else
        {
            address = RnVal - offset;
            debugShifter[0] = '-';
        }

        if(bit_W)
        {
            if (!checkCondition(condCode)) {
                //hh = hh + "(cc not met)";
                DEBUG("CC not met\n");
                return;
            }
            REG(Rn)=address;
        }
    }
    else
    {
        address = RnVal;
        if (!checkCondition(condCode)) {
            DEBUG("CC not met\n");
            return;
        }
        if(bit_U)
        {
            REG(Rn) =  RnVal + offset;
            debugShifter[0] = ',';
        }
        else
        {
            REG(Rn) = RnVal - offset;
            debugShifter[0] = ',';
        }
    }

    uint32_t data;

    if(instr_num == 0) // H,SB,SH
    {
        if(!checkCondition(condCode)) {
            //hh = hh + "(cc not met)";
            DEBUG("CC not met\n");
            return;
        }

        if(bit_H && !bit_S) 		// H
        {
            if((address & 0x1) == 0x0)
            {
                if(bit_L)				// LDRH
                {
                    DEBUG("LDRH %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                    REG(Rd)=mem->read(address,2);
                }
                else					// STRH
                {
                    DEBUG("STRH %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                    mem->write(address,GET_REG(Rd) & 0xFFFF,2);
                }
            }
            else
            {
                printf("@%08x:%08x| Unpredictable\n",old_PC,instruction );
                exit(0);
            }
        }
        else if(!bit_H && bit_S) 	// SB
        {
            if(bit_L)				// LDRSB
            {
                DEBUG("LDRSB %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                data = mem->read(address,1);
                REG(Rd) = signExtend1(data);
            }
            else					// STRSB
            {
                printf("@%08x:%08x| Undefined STRSB\n",old_PC,instruction);
                exit(0);
            }
        }
        else 						//SH
        {
            if(bit_L)				// LDRSH
            {
                if((address & 0x1) ==0x0)
            {
                    DEBUG("LDRSH %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                    data = mem->read(address,2);
                    REG(Rd)=signExtend2(data);
                }
                else
                {
                    printf("@%08x:%08x| Unpredictable\n",old_PC,instruction);
                    exit(0);
                }
            }
            else					// STRSH
            {
                printf("@%08x:%08x| Undefined STRSH \n",old_PC,instruction);
                exit(0);
            }
        }
    }
    else
    {
        if(!checkCondition(condCode)) {
            //hh = hh + "(cc not met)";
            DEBUG("CC not met\n");
            return;
        }

        if(!bit_P  && bit_W) 		// BT,T
        {
            if(bit_B)				// BT
            {
                    if(bit_L)				// LDRBT
                    {
                        DEBUG("LDRBT %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                        REG(Rd) = mem->read(address,1);
                    }
                    else					// STRBT
                    {
                        DEBUG("STRBT %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                        mem->write(address,GET_REG(Rd) & 0xFF,1);
                    }
            }
            else					// T
            {
                if(bit_L)				// LDRT
                {
                    DEBUG("LDRT %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                        
                    data=mem->read(address,4);
                    int tst = address & 0x3;
                    switch(tst)
                    {
                        case 0x0:
                            REG(Rd)=data;
                            break;
                        case 0x1:
                            REG(Rd)=(data << 24) | (data >> 8);
                            break;
                        case 0x2:
                            REG(Rd)=(data << 16) | (data >> 16);
                            break;
                        case 0x3:
                            REG(Rd)=(data << 8) | (data >> 24);
                            break;
                    }
                }
                else					// STRT
                {
                    DEBUG("STRT %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                        
                    mem->write(address,GET_REG(Rd),4);
                }
            }
        }
        else						// -,B
        {
            if(!bit_B)				// -
            {
                if(bit_L)				// LDR
                {
                    DEBUG("LDR %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                        
                    data=mem->read(address,4);
                    int tst = address & 0x3;
                    uint32_t value=0;
        #if 0
                    switch(tst)
                    {
                        case 0x0:
                            value=data;
                            break;
                        case 0x1:
                            value=(data << 24) | (data >> 8);
                            printf("ROT\n");
                            break;
                        case 0x2:
                            value=(data << 16) | (data >> 16);
                            printf("ROT\n");
                            break;
                        case 0x3:
                            value=(data << 8) | (data >> 24);
                            printf("ROT\n");
                            break;
                    }
        #else
                    value=data;
        #endif
                    if(Rd==15)
                    {
                        //printf("@%08x:%08x| LDR loaded: 0x%08x org: 0x%08x\n",old_PC,instruction, value,address);
                        if(value & 0x1)
                        {
                            SET_FLAG(T_MASK);
                        }
                        else
                        {
                            CLR_FLAG(T_MASK);
                        }
                        REG(Rd) = value & 0xFFFFFFFE;
                    }
                    else
                        REG(Rd)=value;
                }
                else					// STR
                {
                    DEBUG("STR %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                        
                    mem->write(address,GET_REG(Rd),4);
                }
            }
            else					// B
            {
                if(bit_L)				// LDRB
                {
                    DEBUG("LDRB %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                        
                    REG(Rd)=mem->read(address,1);
                }
                else					// STRB
                {
                    DEBUG("STRB %s, %s %s (0x%08x)\n" ,RR(Rd),RR(Rn),debugShifter,address);
                        
                    mem->write(address,GET_REG(Rd) & 0xFF,1);
                }
            }
        }
    }
}
