/* 
*   include/pipes.h
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __PIPES_H
#define __PIPES_H

#include <kernel/errors.h>

#define PIPE_SIZE 16384
#define PIPE_SIZE_MASK 16383

typedef struct _PIPE
{
	unsigned long nReceiver;
	unsigned long nSender;
	unsigned char buffer [PIPE_SIZE];
} PIPE;

extern PIPE* g_pKernelCtrlPipe;
extern PIPE* g_pSystemCtrlPipe;
extern PIPE* g_pGFXManagerPipe;

extern void kpipe_write (PIPE* pPipe, void* _pData, unsigned long nBytes);
extern void kpipe_read  (PIPE* pPipe, void* _pData, unsigned long nBytes);

#endif
