/* 
*   kernel/swi_file.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

    case nAPI_RUN_GRV:          //(const char* pGRVPath)
    {
            ERROR_CODE code = load_bflat ((const char *)nParam1);
            return code;
    }
    break;
    
    case nAPI_FILE:
        return fs_swi((int)nParam1,(void *)nParam2, (void *)nParam3);
    




