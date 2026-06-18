
void arm_MSR_MRS(int condCode,int instr_num,uint32_t instruction)
{
    if (checkCondition(condCode))
    {   
        int old_mode = MODE;
        
        if((instruction >> 21) & 0x1)           /* MSR */
        {
            uint32_t  opVal;
            int Rm=instruction&0xF;
            DEBUG("MSR ");            
            
            if((instruction>>25) & 0x1)
            {
                opVal=((instruction&0xFF) << (32-(((instruction>>8) & 0xF)*2))) | ((instruction&0xFF) >> (((instruction>>8) & 0xF)*2));
                DEBUG("[%08x] -> ",opVal);
            }
            else
            {
                opVal = GET_REG(Rm);
                DEBUG("R%d (%08x) -> ",Rm,GET_REG(Rm));
            }
            int mask = (instruction >> 16) & 0xF;
            if((instruction >> 22) & 0x1)
            {
                
                DEBUG("SPSR (mask =%08x)",mask);
                if(old_mode == M_USER || old_mode == M_SYS)
                {
                    DEBUG(" !! ERROR, no SPSR in %s mode\n",mode_str[old_mode]);
                }
                else
                {
                    if( (mask & 0x1) == 0x1)
                        REG(R_SPSR) = (GET_REG(R_SPSR) & ~0x000000FF) | (opVal & 0x000000FF);
                    if( (mask & 0x2) == 0x2)
                        REG(R_SPSR) = (GET_REG(R_SPSR) & ~0x0000FF00) | (opVal & 0x0000FF00);
                    if( (mask & 0x4) == 0x4)
                        REG(R_SPSR) = (GET_REG(R_SPSR) & ~0x00FF0000) | (opVal & 0x00FF0000);
                    if( (mask & 0x8) == 0x8)
                        REG(R_SPSR) = (GET_REG(R_SPSR) & ~0xFF000000) | (opVal & 0xFF000000);
                    DEBUG(" => %08x\n",GET_REG(R_SPSR));
                }
            }
            else
            {
                DEBUG("CPSR (%08x)",mask);
                uint32_t R_CPSR_SAV=GET_REG(R_CPSR);
                uint32_t R_CPSR_CUR;
                
                if( (mask & 0x1) == 0x1)
                {
                    REG(R_CPSR) = (GET_REG(R_CPSR) & ~0x000000FF) | (opVal & 0x000000FF);
                    R_CPSR_CUR=GET_REG(R_CPSR);
                    DEBUG(" => %08x\n",GET_REG(R_CPSR));
                    DEBUG_HW(CPU_DEBUG,"%s",((R_CPSR_SAV&0x80) == (R_CPSR_CUR&0x80)) ? "" : (R_CPSR_CUR&0x80)?"DISABLE IRQ\n":"ENABLE IRQ\n");
                    DEBUG_HW(CPU_DEBUG,"%s",((R_CPSR_SAV&0x40) == (R_CPSR_CUR&0x40)) ? "" : (R_CPSR_CUR&0x40)?"DISABLE FIQ\n":"ENABLE FIQ\n");
                }
                else
                {
                    if( (mask & 0x2) == 0x2)
                        REG(R_CPSR) = (GET_REG(R_CPSR) & ~0x0000FF00) | (opVal & 0x0000FF00);
                    if( (mask & 0x4) == 0x4)
                        REG(R_CPSR) = (GET_REG(R_CPSR) & ~0x00FF0000) | (opVal & 0x00FF0000);
                    if( (mask & 0x8) == 0x8)
                        REG(R_CPSR) = (GET_REG(R_CPSR) & ~0xFF000000) | (opVal & 0xFF000000);
                    DEBUG(" => %08x\n",GET_REG(R_CPSR));
                }
            }
            CHK_T_FLAG_FCT
            CHK_IRQ_FCT
            CHK_FIQ_FCT
        }
        else                                   /* MRS */
        {
            DEBUG("MRS ");
            
            int Rd = (instruction >> 12) & 0xF;
            if((instruction >> 22) & 0x1)             /* SPSR */
            {
                 if(old_mode == M_USER || old_mode == M_SYS)
                {
                    DEBUG("ERROR, no SPSR in %s mode\n",mode_str[old_mode]);
                    
                }
                else
                {
                    DEBUG("R%d <- SPSR (%08x)\n",Rd,GET_REG(R_SPSR));
                    REG(Rd) = GET_REG(R_SPSR);
                }
                
            }
            else                                      /* CPSR */
            {
                DEBUG("R%d <- CPSR (%08x)\n",Rd,GET_REG(R_CPSR));
                REG(Rd) = GET_REG(R_CPSR);
            }
        }
        
        /* checking mode change */
        if(old_mode != MODE)
        {
            DEBUG("Mode has changed from %s to %s\n",mode_str[old_mode],mode_str[MODE]);
            current_reg = mode_regs[MODE];
        }
    }
    else
    {
        DEBUG("MSR/MRS CC not met\n");
    }
    
    
            
}

void arm_CoProcessor(int condCode,uint32_t instruction)
{
    INT_DEBUG_HEAD
    printf("coprocessor instruction: %08x\n",instruction);
    exit(0);
}


void arm_DSP(int condCode,uint32_t instruction)
{
    INT_DEBUG_HEAD
    printf("DSP instruction: %08x\n",instruction);
    exit(0);
}

void arm_Swap(uint32_t instruction)
{
    int Rm=instruction & 0xF;
    int Rn=(instruction>>16) &0xF;
    int Rd=(instruction>>12) &0xF;
    
    uint32_t data;  
    
    DEBUG("SWP"); 

    if((instruction>>22) & 0x1)           // SWPB
    {
        DEBUG("B [R%d] = R%d R%d = [R%d]\n",Rn,Rm,Rd);
        data = mem->read(GET_REG(Rn),1);
        mem->write(GET_REG(Rn),GET_REG(Rm) & 0xFF,1);
        REG(Rd) = data;
    }
    else                                  // SWP
    {
        DEBUG(" [R%d] = R%d R%d = [R%d] (rot right:",Rn,Rm,Rd);
        switch(GET_REG(Rn) & 0x3)
        {
            case 0x0:
                data = mem->read(GET_REG(Rn),4);
                DEBUG("0)\n");
                break;
            case 0x1:
                data = mem->read(GET_REG(Rn),4);
                data = ((data << 24) & 0xFF000000) | ((data >> 8) & 0x00FFFFFF);
                DEBUG("8)\n");
                break;
            case 0x2:
                data = mem->read(GET_REG(Rn),4);
                data = ((data << 16) & 0xFFFF0000)  | ((data >> 16) & 0x0000FFFF);
                DEBUG("16)\n");
                break;
            case 0x3:
                data = mem->read(GET_REG(Rn),4);
                data = ((data << 8) & 0xFFFFFF00)  | ((data >> 24) & 0xFF000000);
                DEBUG("24)\n");
                break;
        }
        mem->write(GET_REG(Rn),GET_REG(Rm),4);
        REG(Rd)=data;
    }
}
