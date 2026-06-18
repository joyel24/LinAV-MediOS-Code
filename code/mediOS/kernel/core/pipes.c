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

void pipeWrite(struct pipe * pPipe, void* _pData, unsigned long nBytes)
{
    int i;
    unsigned char* pData = (unsigned char*)_pData;
    if(pPipe)
        for (i=0;i<nBytes;i++)
        {
                pPipe->buffer[pPipe->nIN ++] = pData[i];
                pPipe->nIN &= PIPE_SIZE_MASK;
        }
}

void pipeRead(struct pipe* pPipe, void* _pData, unsigned long nBytes)
{
    unsigned char* pOut = (unsigned char*)_pData;
    if(pPipe)
        while (nBytes -- && pPipe->nIN != pPipe->nOUT)
        {
                *pOut++ = pPipe->buffer[pPipe->nOUT ++];
                pPipe->nOUT &= PIPE_SIZE_MASK;
        }
}
