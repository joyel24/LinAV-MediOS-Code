/* 
*   include/errors.h
*
*   AMOS project
*
*/

#ifndef __ERRORS_H
#define __ERRORS_H

typedef enum _KERNEL_ERROR_CODE
{
	eOK       = 0,
	ePOINTER,
	eNOMEMORY,
	eNOTFOUND
} KERNEL_ERROR_CODE;

#endif
