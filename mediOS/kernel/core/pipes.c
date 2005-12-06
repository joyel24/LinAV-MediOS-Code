/*
*   kernel/core/pipes.c
*
*   MediOS project
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

void kpipe_write  (PIPE* pPipe, void* _pData, unsigned long nBytes)
{
    int i;
    unsigned char* pData = (unsigned char*)_pData;
    if(pPipe)
        for (i=0;i<nBytes;i++)
        {
                pPipe->buffer[pPipe->nSender ++] = pData[i];
                pPipe->nSender &= PIPE_SIZE_MASK;
        }
}

void kpipe_read  (PIPE* pPipe, void* _pData, unsigned long nBytes)
{
    unsigned char* pOut = (unsigned char*)_pData;
    if(pPipe)
        while (nBytes -- && pPipe->nReceiver != pPipe->nSender)
        {
                *pOut++ = pPipe->buffer[pPipe->nReceiver ++];
                pPipe->nReceiver &= PIPE_SIZE_MASK;
        }
}
