int formatTable[9][16] = {
    {0x1,0x11} ,
    {0x1,0x11},
    {0xA,0x6,0x1,0x11},
    {0x8,0x9,0x3},
    {0x2,0x7,0x8,0x9,0x3,0x0,0x10,0xF,0x12,0xD,0x6,0x5,0xE,0xB,0x4,0xC},
    {0x1},
    {0x1,0x11},
    {0x1,0x6,0xA},
    {0x5,0x7,0x6,0x3,0x0,0x2,0x1,0x4}};

void thumb_data_process(int format,int opcode,uint32_t instruction)
{
        int Rd=0;
        int Rn=0;
        int op2Reg=0;
        uint32_t shifter_operand=0;
        uint32_t RnVal;
        
        
        uint32_t alu_out=0;
        uint32_t addOn;
        
        char debugShifter[50];

        switch(format)
        {
            case 0x0:
                Rd = instruction & 0x7;            // Rd
                Rn =  (instruction >> 3) & 0x7;     // Rn
                op2Reg =  (instruction >> 6) & 0x7;  // Rm
                shifter_operand = GET_REG(op2Reg);
                MKDEBUG(debugShifter,"%s, %s, %s",RR(Rd),RR(Rn),RR(op2Reg));
                break;
            case 0x1:
                Rd = instruction & 0x7;            // Rd
                Rn =  (instruction >> 3) & 0x7;     // Rn                
                shifter_operand = (instruction >> 6) & 0x7;      //3 bit immed
                MKDEBUG(debugShifter,"%s, %s, 0x%x",RR(Rd),RR(Rn),shifter_operand);
                break;
            case 0x2:
                Rd =  (instruction >> 8) & 0x7;
                Rn = Rd;
                shifter_operand =  instruction & 0xFF;
                MKDEBUG(debugShifter,"%s, 0x%x",RR(Rd),shifter_operand);
                break;
            case 0x3:
                Rd =  instruction & 0x7;                    // Rd
                Rn = (instruction >> 3) & 0x7;               // Rm
                shifter_operand =  (instruction >> 6) & 0x1F;             // shift_imm
                MKDEBUG(debugShifter,"%s, %s, 0x%x",RR(Rd),RR(Rn),shifter_operand);
                break;
            case 0x4:
                Rd =  instruction & 0x7;
                Rn =  Rd;
                op2Reg = (instruction >> 3) & 0x7;
                shifter_operand = GET_REG(op2Reg);
                MKDEBUG(debugShifter,"%s, %s",RR(Rd),RR(op2Reg));
                break;
            case 0x5:
                opcode=0;
                Rd = (instruction >> 8) & 0x7;
                if(TST_BIT(instruction,11))  // SP
                        Rn=R_SP;
                else                         // PC
                        Rn=R_PC;
                shifter_operand = (instruction & 0xFF) << 2;
                MKDEBUG(debugShifter,"%s, %s, 0x%x",RR(Rd),RR(Rn),shifter_operand);
                break;
            case 0x6:
                Rd = R_SP;
                Rn = R_SP;
                shifter_operand =  (instruction & 0x7F) << 2;
                MKDEBUG(debugShifter,"%s, %s, 0x%x",RR(Rd),RR(Rn),shifter_operand);
                break;
            case 0x7:
                Rd = ( instruction & 0x7) | ((instruction & 0x0080) >> 4);
                Rn = Rd;
                op2Reg=( (instruction >> 3) & 0x7) | ((instruction & 0x0040) >> 3);
                shifter_operand = GET_REG(op2Reg);
                MKDEBUG(debugShifter,"%s, %s",RR(Rd),RR(op2Reg));
                break;
        }

        RnVal = GET_REG(Rn);
                
        switch(formatTable[format][opcode])
        {
            case 0x0:                                                                              // ADC
                DEBUG("ADC %s\n",debugShifter);
                addOn=0;
                if(C_FLAG)
                    addOn=1;
                REG(Rd) = RnVal + shifter_operand+addOn;
                ARM_NegZero(GET_REG(Rd));
                ARM_AddCCarry(RnVal,shifter_operand,addOn);
                ARM_AddCOverflow(RnVal,shifter_operand,addOn);
                break;
            case 0x1:                                                                              // ADD
                DEBUG("ADD %s\n",debugShifter);
                if(Rn == R_PC)
                    RnVal &= 0xFFFFFFFC;
                REG(Rd) = RnVal + shifter_operand;
                if(format == 0 || format == 1 || format == 2)
                {
                    ARM_NegZero(GET_REG(Rd));
                    ARM_AddCarry(RnVal,shifter_operand, RnVal + shifter_operand);
                    ARM_AddOverflow(RnVal,shifter_operand, RnVal + shifter_operand);
                }
                break;
            case 0x2:                                                                              // AND
                DEBUG("AND %s\n",debugShifter);
                REG(Rd)=(RnVal & shifter_operand) & 0xFFFFFFFF;
                ARM_NegZero(GET_REG(Rd));
                break;
            case 0x3:                                                                              // ASR
                DEBUG("ASR %s\n",debugShifter);
                shifter_operand &= 0xFF;
                if(format == 3)
                {
                    if(shifter_operand == 0)
                    {
                        SET_C((RnVal >> 31) & 0x1);
                        if(!C_FLAG)
                            REG(Rd) = 0;
                        else
                            REG(Rd) = 0xFFFFFFFF;
                    }
                    else
                    {
                        SET_C((RnVal >> (shifter_operand - 1)) & 0x1);
                        if(RnVal > 0x7FFFFFFF) // neg number => keep neg sign
                        {
                            for (int j = 0; j < shifter_operand; j++) {
                                    RnVal = (RnVal >> 1) | 0x80000000;
                            }
                            REG(Rd) = RnVal;
                        }
                        else
                            REG(Rd) = RnVal >> shifter_operand;
                    }
                    ARM_NegZero(GET_REG(Rd));
                }
                else
                {
                    if(shifter_operand == 0)
                    {
                        /* nothing to do */
                    }
                    else if(shifter_operand  <32)
                    {
                        SET_C((RnVal >> (shifter_operand - 1)) & 0x1);
                        if(RnVal > 0x7FFFFFFF) // neg number => keep neg sign
                        {
                            for (int j = 0; j < shifter_operand; j++) {
                                    RnVal = (RnVal >> 1) | 0x80000000;
                            }
                            REG(Rd) = RnVal;
                        }
                        else
                            REG(Rd) = RnVal >> shifter_operand;
                    }
                    else if(shifter_operand >=32)
                    {
                        SET_C((RnVal >> 31) & 0x1);
                        if(!C_FLAG)
                            REG(Rd) = 0;
                        else
                            REG(Rd) = 0xFFFFFFFF;
                    }
                    ARM_NegZero(GET_REG(Rd));
                }
                break;
            case 0x4:                                                                              // BIC
                DEBUG("BIC %s\n",debugShifter);
                REG(Rd)=RnVal & (~shifter_operand);
                ARM_NegZero(GET_REG(Rd));
                break;
            case 0x5:                                                                              // CMN
                DEBUG("CMN %s\n",debugShifter);
                alu_out=RnVal + shifter_operand; // RnVal - (-shifter_operand)
                ARM_NegZero(alu_out);
                ARM_SubCarry(RnVal,-shifter_operand, alu_out);
                ARM_SubOverflow(RnVal,-shifter_operand, alu_out);
                break;
            case 0x6:                                                                              // CMP
                DEBUG("CMP %s\n",debugShifter);
                alu_out=RnVal - shifter_operand;
                ARM_NegZero(alu_out);
                ARM_SubCarry(RnVal,shifter_operand, alu_out);
                ARM_SubOverflow(RnVal,shifter_operand, alu_out);
                break;
            case 0x7:                                                                              // EOR
                DEBUG("EOR %s\n",debugShifter);
                REG(Rd)=RnVal ^ shifter_operand;
                ARM_NegZero(GET_REG(Rd));
                break;
            case 0x8:                                                                              // LSL
                DEBUG("LSL %s\n",debugShifter);
                if(format == 3)                                 // using immed
                {
                    if(shifter_operand == 0)
                    {
                        REG(Rd)=RnVal;
                    }
                    else
                    {
                        SET_C((RnVal >> (32 - shifter_operand)) & 0x1);
                        REG(Rd)=RnVal << shifter_operand;
                    }
                    ARM_NegZero(GET_REG(Rd));
                }
                else                                          // using Rs
                {
                    if((shifter_operand & 0xFF) == 0)
                    {
                        /* nothing to do */
                    }
                    else if((shifter_operand & 0xFF) < 32)
                    {
                        SET_C((RnVal >> (32 - (shifter_operand & 0xFF))) & 0x1);
                        REG(Rd)=RnVal << (shifter_operand & 0xFF);
                    }
                    else if((shifter_operand & 0xFF) == 32)
                    {
                        SET_C(RnVal & 0x1);
                        REG(Rd) = 0;
                    }
                    else
                    {
                        SET_C(false);
                        REG(Rd) = 0;
                    }
                    ARM_NegZero(GET_REG(Rd));
                }
                break;
            case 0x9:                                                                              // LSR
                DEBUG("LSR %s\n",debugShifter);
                if(format == 3)                                 // using immed
                {
                    if(shifter_operand == 0)
                    {
                        SET_C((RnVal >> 31) & 0x1);
                        REG(Rd)=0;
                    }
                    else
                    {
                        SET_C((RnVal >> (shifter_operand-1)) & 0x1);
                        REG(Rd)=RnVal >> shifter_operand;
                    }
                    ARM_NegZero(GET_REG(Rd));
                }
                else                                          // using Rs
                {
                    if((shifter_operand & 0xFF) == 0)
                    {
                        /* nothing to do */
                    }
                    else if((shifter_operand & 0xFF) < 32)
                    {
                        SET_C((RnVal >> ((shifter_operand & 0xFF)-1)) & 0x1);
                        REG(Rd)=RnVal >> (shifter_operand & 0xFF);
                    }
                    else if((shifter_operand & 0xFF) == 32)
                    {
                        SET_C((GET_REG(Rd) >> 31) & 0x1);
                        REG(Rd) = 0;
                    }
                    else if((shifter_operand & 0xFF) > 32)
                    {
                        SET_C(false);
                        REG(Rd) = 0;
                    }
                    ARM_NegZero(GET_REG(Rd));
                }
                break;
            case 0xA:                                                                              // MOV
                DEBUG("MOV %s\n",debugShifter);
                if(format == 0x7 && op2Reg == R_PC)
                    REG(Rd)=shifter_operand+1;
                else
                    REG(Rd)=shifter_operand;
                if(format == 2 | format == 4)
                {
                    ARM_NegZero(GET_REG(Rd));
                }
                if(format == 4)
                {
                    SET_C(false);
                    SET_V(false);
                }
                break;
            case 0xB:                                                                              // MUL
                DEBUG("MUL %s\n",debugShifter);
                REG(Rd)=RnVal * shifter_operand;
                ARM_NegZero(GET_REG(Rd));
                break;
            case 0xC:                                                                              // MVN
                DEBUG("MVN %s\n",debugShifter);
                REG(Rd)=~shifter_operand;
                ARM_NegZero(GET_REG(Rd));
                break;
            case 0xD:                                                                              // NEG
                DEBUG("NEG %s\n",debugShifter);
                REG(Rd)=0-shifter_operand;
                ARM_NegZero(GET_REG(Rd));
                ARM_SubCarry(0,shifter_operand, 0-shifter_operand);
                ARM_SubOverflow(0,shifter_operand, 0-shifter_operand);
                break;
            case 0xE:                                                                              // ORR
                DEBUG("ORR %s\n",debugShifter);
                REG(Rd)= RnVal | shifter_operand;
                ARM_NegZero(GET_REG(Rd));
                break;
            case 0xF:                                                                              // ROR
                DEBUG("ROR %s\n",debugShifter);
                if((shifter_operand & 0xFF) == 0)
                {
                    /* nothing to do */
                }
                else if((shifter_operand & 0xF) == 0)
                {
                        SET_C((GET_REG(Rd) >> 31) & 0x1);
                }
                else
                {
                        SET_C((RnVal >> ((shifter_operand & 0xF)-1)) & 0x1);
                        REG(Rd)= (RnVal << (32-(shifter_operand & 0xF))) | (RnVal >> (shifter_operand & 0xF));
                }
                ARM_NegZero(GET_REG(Rd));
                break;
            case 0x10:                                                                              // SBC
                DEBUG("SBC %s\n",debugShifter);
                addOn = 0;
                if(C_FLAG)
                    addOn=1;
                addOn = ~addOn;
                REG(Rd) = RnVal - shifter_operand - addOn;

                ARM_NegZero(GET_REG(Rd));
                ARM_SubCOverflow(RnVal,shifter_operand, addOn);
                ARM_SubCCarry(RnVal,shifter_operand, addOn);
                break;
            case 0x11:                                                                              // SUB
                DEBUG("SUB %s\n",debugShifter);
                REG(Rd) = RnVal - shifter_operand;
                if(format != 6) // no flags on SP varient
                {
                    ARM_NegZero(GET_REG(Rd));
                    ARM_SubCarry(RnVal,shifter_operand, RnVal - shifter_operand);
                    ARM_SubOverflow(RnVal,shifter_operand, RnVal - shifter_operand);
                }
                break;
            case 0x12:                                                                              // TST
                DEBUG("TST %s\n",debugShifter);
                alu_out=GET_REG(Rd) & shifter_operand;
                ARM_NegZero(alu_out);
                break;
        }
}
