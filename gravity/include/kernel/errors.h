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

typedef enum _ERROR_CODE
{
	ERR_OK              = 0,
	ERR_FATAL           = 1,
	ERR_NOMEMORY        = 2,
	ERR_INVALID_HANDLE  = 3,
	ERR_NOT_IMPLEMENTED = 4,
	ERR_SOUND_BUSY      = 5,
	ERR_INVALID_PARAM   = 6,
	ERR_INVALID_PATH    = 7,
	ERR_FILE_NOT_FOUND  = 8,
	ERR_END_OF_FILE     = 9,
	ERR_RECEIVER_EMPTY  = 10,
	ERR_NO_GFX_CONTEXT  = 11,
	ERR_NO_FONT         = 12,
} ERROR_CODE;

#endif
