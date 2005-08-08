void arm_DataProcessing(int condCode,uint32_t instruction)
{    
    int Rd = (instruction >> 12) & 0xF;
    int Rn = (instruction >> 16) & 0xF;
    
    char debugShifter[50];
    int shifter_carry_out=0;

    uint32_t shifter_operand=getShifterData(instruction,debugShifter,&shifter_carry_out);

    int opcode = (instruction >> 21 ) & 0xF;
    uint32_t alu_out;

    bool fl_s = (instruction >> 20 ) & 0x1==0x1;

    
                
    if (!checkCondition(condCode))
    {
        DEBUG("CC not met\n");
        return;
    }

    switch(opcode)
    {
        case 0x0: ///////////////////////////////////////////// AND
            DEBUG("AND %s, %s, %s\n",RR(Rd),RR(Rn),debugShifter);
            REG(Rd)=GET_REG(Rn) & shifter_operand;
            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                SET_C(shifter_carry_out == 1);
            }
            break;
        case 0x1: ///////////////////////////////////////////// EOR
            DEBUG("EOR %s, %s, %s\n",RR(Rd),RR(Rn),debugShifter);
            REG(Rd)=GET_REG(Rn) ^ shifter_operand;
            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                SET_C(shifter_carry_out == 1);
            }
            break;
        case 0x2: ///////////////////////////////////////////// SUB
            if(fl_s && Rd==15)
                printf("SUBS %s, %s, %s old_val %x\n",RR(Rd),RR(Rn),debugShifter,GET_REG(Rd));
            
            REG(Rd)=GET_REG(Rn) - shifter_operand;
            DEBUG("SUB%s %s, %s, %s => %x\n",fl_s?"S":"",RR(Rd),RR(Rn),debugShifter,GET_REG(Rd));            
            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                ARM_SubCarry(GET_REG(Rn),shifter_operand, GET_REG(Rn) - shifter_operand);
                ARM_SubOverflow(GET_REG(Rn),shifter_operand, GET_REG(Rn) - shifter_operand);
            }
            break;
        case 0x3: ///////////////////////////////////////////// RSB => reverse subtract
            DEBUG("RSB%s %s, %s, %s\n",fl_s?"S":"",RR(Rd),RR(Rn),debugShifter);
            REG(Rd)=shifter_operand-GET_REG(Rn);
            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                ARM_SubCarry(shifter_operand,GET_REG(Rn), shifter_operand-GET_REG(Rn));
                ARM_SubOverflow(shifter_operand,GET_REG(Rn), shifter_operand-GET_REG(Rn));
            }
            break;
        case 0x4: ///////////////////////////////////////////// ADD
            DEBUG("ADD%s %s, %s, %s\n",fl_s?"S":"",RR(Rd),RR(Rn),debugShifter);
            REG(Rd)=GET_REG(Rn) + shifter_operand;
            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                ARM_AddCarry(GET_REG(Rn),shifter_operand, GET_REG(Rn) + shifter_operand);
                ARM_AddOverflow(GET_REG(Rn),shifter_operand, GET_REG(Rn) + shifter_operand);
            }
            break;
        case 0x5: ///////////////////////////////////////////// ADC => add with carry
            DEBUG("ADC%s %s, %s, %s\n",fl_s?"S":"",RR(Rd),RR(Rn),debugShifter);
            int addOn;
            if(C_FLAG)
                addOn=1;
            else
                addOn=0;

            REG(Rd)=GET_REG(Rn) + shifter_operand + addOn;
            if(C_FLAG)
            {
                REG(Rd)=GET_REG(Rn)+1;
            }

            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                ARM_AddCOverflow(GET_REG(Rn),shifter_operand,addOn); // need to be done first
                ARM_AddCCarry(GET_REG(Rn),shifter_operand,addOn);
            }
            break;
        case 0x6: ///////////////////////////////////////////// SBC => sub with carry
            DEBUG("SBC%s %s, %s, %s\n",fl_s?"S":"",RR(Rd),RR(Rn),debugShifter);
            if(C_FLAG)
                addOn=1;
            else
                addOn=0;
            addOn = ~ addOn;
            REG(Rd)=GET_REG(Rn) - shifter_operand - addOn;

            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                ARM_SubCOverflow(GET_REG(Rn),shifter_operand, addOn);
                ARM_SubCCarry(GET_REG(Rn),shifter_operand, addOn);
            }
            break;
        case 0x7: ///////////////////////////////////////////// RSC => reverse subtract with carry
            DEBUG("RSC%s %s, %s, %s\n",fl_s?"S":"",RR(Rd),RR(Rn),debugShifter);
            if(C_FLAG)
                addOn=1;
            else
                addOn=0;
            addOn = ~ addOn;
            REG(Rd)=shifter_operand - GET_REG(Rn) - addOn;

            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                ARM_SubCOverflow(shifter_operand,GET_REG(Rn),addOn);
                ARM_SubCCarry(shifter_operand,GET_REG(Rn),addOn);
            }
            break;
        case 0x8: ///////////////////////////////////////////// TST => Rn & shifter_operand
            DEBUG("TST %s, %s\n",RR(Rn),debugShifter);
            alu_out=GET_REG(Rn) & shifter_operand;

            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(alu_out);
                SET_C(shifter_carry_out == 1);
            }
            break;
        case 0x9: ///////////////////////////////////////////// TEQ => Rn ^ shifter_operand
            DEBUG("TEQ %s, %s\n",RR(Rn),debugShifter);
            alu_out=GET_REG(Rn) ^ shifter_operand;

            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(alu_out);
                SET_C(shifter_carry_out == 1);
            }
            break;
        case 0xA: ///////////////////////////////////////////// CMP => Rn - shifter_operand
            DEBUG("CMP %s, %s\n",RR(Rn),debugShifter);
            alu_out=GET_REG(Rn) - shifter_operand;

            if(fl_s)
            {
                ARM_NegZero(alu_out);
                ARM_SubCarry(GET_REG(Rn),shifter_operand, alu_out);
                ARM_SubOverflow(GET_REG(Rn),shifter_operand, alu_out);
            }
            break;
        case 0xB: ///////////////////////////////////////////// CMN => Rn + shifter_operand
            DEBUG("CMN %s, %s\n",RR(Rn),debugShifter);
            alu_out=GET_REG(Rn) + shifter_operand;

            if(fl_s)
            {
                ARM_NegZero(alu_out);
                ARM_SubCarry(GET_REG(Rn),-shifter_operand, alu_out);
                ARM_SubOverflow(GET_REG(Rn),-shifter_operand, alu_out);
            }
            else
                printf(" fl_s not set!!!\n");
            break;
        case 0xC: ///////////////////////////////////////////// ORR
            DEBUG("ORR %s, %s, %s\n",RR(Rd),RR(Rn),debugShifter);
            REG(Rd)=GET_REG(Rn) | shifter_operand;

            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                SET_C(shifter_carry_out == 1);
            }
            break;
        case 0xD: ///////////////////////////////////////////// MOV
            DEBUG("MOV%s %s, %s\n",fl_s?"S":"",RR(Rd),debugShifter);
            REG(Rd)=shifter_operand;

            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                SET_C(shifter_carry_out == 1);
            }
            break;
        case 0xE: ///////////////////////////////////////////// BIC => Bit clear
            DEBUG("BIC %s, %s, %s\n",RR(Rd),RR(Rn),debugShifter);
            REG(Rd)=GET_REG(Rn) & (~shifter_operand);
            //printf("R dest(%d) = %08x %x %x\n",Rd,GET_REG(Rd),shifter_operand,~shifter_operand);
            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                SET_C(shifter_carry_out == 1);
            }
            break;
        case 0xF: ///////////////////////////////////////////// MVN => MOV not
            DEBUG("MVN%s %s, %s\n",fl_s?"S":"",RR(Rd),debugShifter);
            REG(Rd)=~shifter_operand;

            if(fl_s && Rd==15)
            {
                CHG_MODE
            }
            else if(fl_s)
            {
                ARM_NegZero(GET_REG(Rd));
                SET_C(shifter_carry_out == 1);
            }
            break;
        }
}

uint32_t getShifterData(uint32_t instruction,char *debugShifter,int *shifter_carry_out)
{

    uint32_t shifter_operand=0;
    *shifter_carry_out=0;

    if((instruction >> 25) & 0x1) // 32-bit immediate
    {
            int immed_8 = instruction & 0xFF;
            int rotate_imm = (instruction >> 8) & 0xF;
            shifter_operand = ((immed_8 << (32-rotate_imm*2)) | (immed_8 >> (rotate_imm*2))) ;
            if(rotate_imm == 0)
                if(C_FLAG)
                    *shifter_carry_out = 1;
                else
                    *shifter_carry_out = 0;
            else
                *shifter_carry_out = (shifter_operand >> 31) & 0x1;
            MKDEBUG(debugShifter,"0x%x",shifter_operand);
    }
    else
    {
        int Rm = instruction & 0xF;
        int shift = (instruction >> 5) & 0x3;
        uint32_t RmVal=GET_REG(Rm);

        if(((instruction >> 4) & 0x1)==0) // immediate shifts
        {
            int shift_imm = (instruction >> 7) & 0x1F;

            switch(shift)
            {
                case  0x0:                            // logical shift left
                    if(shift_imm == 0)
                    {
                        shifter_operand=RmVal;
                        if(C_FLAG)
                            *shifter_carry_out = 1;
                        else
                            *shifter_carry_out = 0;
                        MKDEBUG(debugShifter,"%s",RR(Rm));
                    }
                    else
                    {
                        MKDEBUG(debugShifter,"%s, LSL 0x%x",RR(Rm),shift_imm);
                        shifter_operand = RmVal << shift_imm;
                        *shifter_carry_out = (RmVal >> (32-shift_imm)) & 0x1;
                    }
                    break;
                case  0x1:                            // logical shift right
                    MKDEBUG(debugShifter,"%s, LSR 0x%x",RR(Rm),shift_imm);
                    if(shift_imm == 0)
                    {
                        shifter_operand=0;
                        *shifter_carry_out = (RmVal >> 31) & 0x1;
                    }
                    else
                    {
                        shifter_operand =  RmVal >> shift_imm;
                        *shifter_carry_out = (RmVal >> (shift_imm-1)) & 0x1;
                    }
                    break;
                case 0x2:                            // arithmetic shift right
                    MKDEBUG(debugShifter,"%s, ASR 0x%x",RR(Rm),shift_imm);
                    if(shift_imm == 0)
                    {
                        if(((RmVal >> 31) & 0x1) == 0)
                        {
                            shifter_operand=0;
                            *shifter_carry_out = 0; // == Rm[31]
                        }
                        else
                        {
                            shifter_operand=0xFFFFFFFF;
                            *shifter_carry_out = 1; // == Rm[31]
                        }
                    }
                    else
                    {
                        uint32_t savRm=RmVal;
                        if(RmVal > 0x7FFFFFFF) // neg number => keep neg sign
                        {
                            for (int j = 0; j < shift_imm; j++)
                            {
                                    RmVal = (RmVal >> 1) | 0x80000000;
                            }
                            shifter_operand = RmVal;
                        }
                        else
                                shifter_operand = RmVal >> shift_imm;

                        *shifter_carry_out = (savRm >> (shift_imm-1)) & 0x1;
                    }
                    break;
                case 0x3:                            // rotate right
                    if(shift_imm == 0) // with extend
                    {
                        MKDEBUG(debugShifter,"%s, RRX",RR(Rm));
                        shifter_operand = RmVal >> 1;
                        if(C_FLAG)
                            shifter_operand = shifter_operand | 0x80000000;
                        *shifter_carry_out =  RmVal  & 0x1;
                    }
                    else // normal rotate
                    {
                        MKDEBUG(debugShifter,"%s, ROR 0x%x",RR(Rm),shift_imm);
                        shifter_operand = (RmVal << (32-shift_imm))
                                | (RmVal >> shift_imm);
                        *shifter_carry_out = (RmVal >> (shift_imm-1)) & 0x1;
                    }
                    break;
            }
        }
        else // register shifts
        {
            int Rs = (instruction >> 8) & 0xF;

            if(Rs == 0 && shift == 0) // no shift
            {
                shifter_operand = GET_REG(Rm);
                if(C_FLAG)
                    *shifter_carry_out = 1;
                else
                    *shifter_carry_out = 0;
                MKDEBUG(debugShifter,"%s",RR(Rm));
            }
            else
            {
                uint32_t RsVal=GET_REG(Rs) & 0xff;
                RmVal=GET_REG(Rm);
                switch(shift)
                {
                    case 0x0:                            // logical shift left
                        MKDEBUG(debugShifter,"%s, LSL %s (0x%x)",RR(Rm),RR(Rs),RsVal);
                        if(RsVal == 0)
                        {
                            shifter_operand = RmVal;
                            if(C_FLAG)
                                *shifter_carry_out = 1;
                            else
                                *shifter_carry_out = 0;
                        }
                        else if(RsVal < 32)
                        {
                            shifter_operand = RmVal << RsVal;
                            *shifter_carry_out = (RmVal >> (32-RsVal)) & 0x1;
                        }
                        else if(RsVal == 32)
                        {
                            shifter_operand = 0;
                            *shifter_carry_out = RmVal & 0x1;
                        }
                        else
                        {
                            shifter_operand = 0;
                            *shifter_carry_out = 0;
                        }
                        break;
                    case 0x1:                            // logical shift right
                        MKDEBUG(debugShifter,"%s, LSR %s (0x%x)",RR(Rm),RR(Rs),RsVal);
                        if(RsVal == 0)
                        {
                            shifter_operand = RmVal;
                            if(C_FLAG)
                                *shifter_carry_out = 1;
                            else
                                *shifter_carry_out = 0;
                        }
                        else if(RsVal < 32)
                        {
                            shifter_operand = RmVal >> RsVal;
                            *shifter_carry_out = (RmVal >> (RsVal-1)) & 0x1;
                        }
                        else if(RsVal == 32)
                        {
                            shifter_operand = 0;
                            *shifter_carry_out = (RmVal >> 31) & 0x1;
                        }
                        else
                        {
                            shifter_operand = 0;
                            *shifter_carry_out = 0;
                        }
                        break;
                    case 0x2:                            // arithmetic shift right
                        MKDEBUG(debugShifter,"%s, ASR %s (0x%x)",RR(Rm),RR(Rs),RsVal);
                        if(RsVal == 0)
                        {
                            shifter_operand = RmVal;
                            if(C_FLAG)
                                *shifter_carry_out = 1;
                            else
                                *shifter_carry_out = 0;
                        }
                        else if(RsVal < 32)
                        {
                            uint32_t savRm=RmVal;
                            if(RmVal > 0x7FFFFFFF) // neg number => keep neg sign
                            {
                                for (int j = 0; j < RsVal; j++) {
                                    RmVal = (RmVal >> 1) | 0x80000000;
                                }
                                shifter_operand = RmVal;
                            }
                            else
                                shifter_operand = RmVal >> RsVal;

                            *shifter_carry_out = (savRm >> (RsVal-1)) & 0x1;
                        }
                        else if(RsVal >= 32)
                        {
                            if(((RmVal >> 31) & 0x1) == 0x0)
                            {
                                    shifter_operand = 0x0;
                                    *shifter_carry_out = 0x0;	// Rm[31]
                            }
                            else
                            {
                                    shifter_operand = 0xFFFFFFFF;
                                    *shifter_carry_out = 0x1;	// Rm[31]
                            }
                        }
                        break;
                    case 0x3:                            // rotate right
                        MKDEBUG(debugShifter,"%s, ROR %s (0x%x)",RR(Rm),RR(Rs),RsVal);
                        if(RsVal == 0)
                        {
                            shifter_operand = RmVal;
                            if(C_FLAG)
                                *shifter_carry_out = 1;
                            else
                                *shifter_carry_out = 0;
                        }
                        else if((RsVal & 0xF) ==0)
                        {
                            shifter_operand =(int) RmVal;
                            *shifter_carry_out = (int) (RmVal >> 31) & 0x1;
                        }
                        else
                        {
                            shifter_operand = (RmVal << (32-(RsVal & 0xF)))
                                    | (RmVal >> (RsVal & 0xF));
                            *shifter_carry_out = RmVal >> ((RsVal & 0xF)-1) & 0x1;
                        }
                    }
            }
        }
    }
    return  shifter_operand;
}
