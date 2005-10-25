/* 
*   kernel/swi_file.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <kernel/bflat.h>

int fs_swi(int cmd,void * data1, void * data2);

__IRAM_CODE int swi_file_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
            case nAPI_RUN_GRV:          //(const char* pGRVPath)
            {
                    ERROR_CODE code = load_bflat ((const char *)nParam1);
                    return code;
            }
            break;
            
            case nAPI_FILE:
                return fs_swi((int)nParam1,(void *)nParam2, (void *)nParam3);
            default:
                printk("File undefined swi (%d)\n",nCmd);
        }
        return 0;
}
    




