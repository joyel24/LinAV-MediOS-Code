/* 
*   kernel/pipes.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/kernel.h>
#include <kernel/pipes.h>

PIPE* g_pKernelCtrlPipe __IRAM_DATA = 0;
PIPE* g_pSystemCtrlPipe __IRAM_DATA = 0;

__IRAM_CODE void kpipe_write  (PIPE* pPipe, void* _pData, unsigned long nBytes)
{
	if (!pPipe)
		return;

	int i;
	unsigned char* pData = (unsigned char*)_pData;
	for (i=0;i<nBytes;i++)
	{
		pPipe->buffer[pPipe->nSender ++] = pData[i];
		pPipe->nSender &= PIPE_SIZE_MASK;
	}
}
