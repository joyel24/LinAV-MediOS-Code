void thumb_load_store(int format,int opcode,uint32_t instruction)
{   
    int Rd;
    int Rn;
    int op2Reg;
    
    uint32_t op2;
    uint32_t address;
    
    int r_op;
    
    char debugShifter[50];

    switch(format)
    {
        case 0x0:
            r_op=(opcode>>1)&0xF;
            Rd =  instruction & 0x7;
            Rn =  (instruction >> 3) & 0x7;
            op2 = ((instruction >> 6) & 0x1F)*(r_op==6?4:r_op==8?2:1);
            opcode = (r_op)-6+5*(~opcode&0x1);
            MKDEBUG(debugShifter,"%s, [%s, 0x%08x]",RR(Rd),RR(Rn),op2);
            break;
        case 0x1:
            opcode = formatTable[8][opcode];
            Rd = instruction & 0x7;
            Rn =  (instruction >> 3) & 0x7;
            op2Reg = (instruction >> 6) & 0x7;
            op2=GET_REG(op2Reg);
            MKDEBUG(debugShifter,"%s, [%s, %s]",RR(Rd),RR(Rn),RR(op2Reg));
            break;
        case 0x2:
            opcode = 0;
            Rd = (instruction >> 8) & 0x7;
            Rn = R_PC;
            op2 = (instruction  & 0xFF)*4;
            MKDEBUG(debugShifter,"%s, [%s, 0x%08x]",RR(Rd),RR(Rn),op2);            
            break;
        case 0x3:
            Rd = (instruction >> 8) & 0x7;
            Rn = R_SP;
            opcode=opcode==1?0:5;
            op2 = (instruction & 0xFF)*4;
            MKDEBUG(debugShifter,"%s, [%s, 0x%08x]",RR(Rd),RR(Rn),op2);
            break;            
    }
    
    switch(opcode)
    {
        case 0:                                                                     // ldr
            DEBUG("LDR %s\n",debugShifter);
            if(Rn == R_PC)
            {
                REG(Rd) = mem->read((GET_REG(15) & 0xFFFFFFFC) + op2,4);
            }
            else
            {
                    address = GET_REG(Rn) + op2;
                    if((address & 0x3) == 0)
                        REG(Rd) = mem->read(address,4);
                    else
                    {
                        DEBUG("Unpredictable LDR\n");
                        exit(0);
                    }
            }
            break;
        case 1:                                                                     // ldrB
            DEBUG("LDRB %s\n",debugShifter);
            REG(Rd) = mem->read(GET_REG(Rn)+ op2,1);
            break;
        case 2:                                                                     // ldrH
            DEBUG("LDRH %s\n",debugShifter);
            address = GET_REG(Rn) + op2;
            if((address & 0x1) == 0)
                REG(Rd) = mem->read(address,2);
            else
            {
                DEBUG("Unpredictable LDR\n");
                exit(0);
            }
            break;
        case 3:                                                                     // ldrSB
            DEBUG("LDRSB %s\n",debugShifter);
            REG(Rd) = signExtend1(mem->read(GET_REG(Rn)+ op2,1));
            break;
        case 4:                                                                     // ldrSH
            DEBUG("LDRSH %s\n",debugShifter);
            REG(Rd) = signExtend2(mem->read(GET_REG(Rn)+ op2,2));
            break;
        case 5:                                                                     // str
            DEBUG("STR %s\n",debugShifter);
            address = GET_REG(Rn) + op2;
            if((address & 0x3) == 0)
                    mem->write(address,GET_REG(Rd),4);
            else
            {
                DEBUG("Unpredictable LDR\n");
                exit(0);
            }
            break;
        case 6:                                                                     // strb
            DEBUG("STRG %s\n",debugShifter);
            mem->write(GET_REG(Rn)+ op2,GET_REG(Rd) & 0xFF,1);
            break;
        case 7:                                                                     // strh
            DEBUG("STRH %s\n",debugShifter);
            address = GET_REG(Rn) + op2;
            if((address & 0x1) == 0)
                mem->write(address,GET_REG(Rd) & 0xFFFF,2);
            else
            {
                DEBUG("Unpredictable LDR\n");
                exit(0);
            }
            break;
    }    
}

