/* 
*   kernel/swi.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>

__IRAM_CODE int kcswi_handler (
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3,
	unsigned long nCmd)
{
    return 0;
}
