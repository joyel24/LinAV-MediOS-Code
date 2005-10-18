        case nAPI_DSP_OPEN:             //(void* pHandler);
        {
                g_pDSPHandler = (DSP_HANDLER)nParam1;
                enable_irq (IRQ_DSP);
        }
        break;
    
        case nAPI_DSP_CLOSE:            //();
        {
                dsp_off ();
                disable_irq (IRQ_DSP);
        }
        break;
    
        case nAPI_DSP_LOAD_MEMCODE:     //(void* pCode, int nSize);
        {
                load_dsp_program_mem ((void*)nParam1, nParam2);
        }
        break;
    
        case nAPI_DSP_LOAD_HDDCODE:     //(const char* pszCoffProgram);
        {
                load_dsp_program_hdd ((const char*)nParam1);
        }
        break;
    
        case nAPI_DSP_ON:               //();
        {
                dsp_on ();
        }
        break;
    
        case nAPI_DSP_OFF:              //();
        {
                dsp_off ();
        }
        break;
    
        case nAPI_DSP_RESET:            //();
        {
                dsp_reset ();
        }
        break;
    
        case nAPI_DSP_RUN:              //();
        {
                dsp_run ();
        }
        break;
