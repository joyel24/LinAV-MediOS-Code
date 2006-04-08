/* 
*   include/kernel/pipes.h
*
*   MediOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __PIPES_H
#define __PIPES_H

#define PIPE_SIZE 16384
#define PIPE_SIZE_MASK 16383

struct pipe
{
    unsigned long nIN;
    unsigned long nOUT;
    unsigned char buffer [PIPE_SIZE];
};


extern void pipeWrite (struct pipe* pPipe, void* _pData, unsigned long nBytes);
extern void pipeRead  (struct pipe* pPipe, void* _pData, unsigned long nBytes);

#endif
