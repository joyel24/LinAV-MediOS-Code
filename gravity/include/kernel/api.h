/* 
*   include/api.h
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __API_H
#define __API_H


typedef struct _POINT
{
	long x;
	long y;
} POINT;

typedef struct _RECT
{
	long x;
	long y;
	long w;
	long h;
} RECT;

typedef struct _IMAGE
{
	void* pixels;
	long w;
	long h;
	long stride;
	long pixel_size;
	long row_order; // 0=top first, 1=bottom first
} IMAGE;

// Interrupt numbers
#define nAPI_TASK_YIELD       0
#define nAPI_TASK_CREATE      1
#define nAPI_TASK_REMOVE      2
#define nAPI_TASK_SUSPEND     3
#define nAPI_TASK_CONTINUE    4
#define nAPI_TASK_GETHANDLE   5
#define nAPI_SLEEP            6

#define nAPI_GET_OS_VER      10
#define nAPI_GETTIME         11
#define nAPI_GETLASTERROR    12
#define nAPI_GETSYSTEMVAL    13
#define nAPI_SETSYSTEMVAL    14
#define nAPI_POWERSAVE       15
#define nAPI_SHUTDOWN        16

#define nAPI_MALLOC          20
#define nAPI_FREE            21
#define nAPI_MEMAVAIL        22
#define nAPI_MEMSET          23
#define nAPI_MEMCPY          24

#define nAPI_PIPE_CREATE     30
#define nAPI_PIPE_DELETE     31
#define nAPI_PIPE_SEND       32
#define nAPI_PIPE_RECV       33

#define nAPI_FPU_ADD         40
#define nAPI_FPU_SUB         41
#define nAPI_FPU_MUL         42
#define nAPI_FPU_DIV         43

#define nAPI_TEXTOUT         50
#define nAPI_SETBKCOLOR      51
#define nAPI_SETTEXTCOLOR    52
#define nAPI_SETFONT         53

#define nAPI_LINE            60
#define nAPI_RECT            61
#define nAPI_FILLRECT        62
#define nAPI_BITBLT          63
#define nAPI_STRETCHBLT      64
#define nAPI_TRANSPBLT       65
#define nAPI_PATBLT          66

#define nAPI_PLAYSOUND       70
#define nAPI_ISPLAYING       71
#define nAPI_STOPSOUND       72
#define nAPI_GETSOUNDPARAM   73
#define nAPI_SETSOUNDPARAM   74
#define nAPI_BYTESPLAYED     75

#define nAPI_SECTOR_READ     80
#define nAPI_SECTOR_WRITE    81
#define nAPI_DRIVES_ENUM     82
#define nAPI_FINDFIRST_FILE  83
#define nAPI_FINDNEXT_FILE   84
#define nAPI_FILE_OPEN       85
#define nAPI_FILE_READ       86
#define nAPI_FILE_WRITE      87




#define SAVE asm volatile ("stmdb sp!, {r3-r12}")
#define LOAD asm volatile ("ldmia sp!, {r3-r12}")


/*
// API definition
inline AOS_RESULT AOS_API_TASK_YIELD       ()                                                              { SAVE; asm("swi 0"); LOAD; }
inline AOS_RESULT AOS_API_TASK_CREATE      (void* pvCode, void* pParam, HANDLE* phTask)                    { SAVE; asm("swi 1"); LOAD; }
inline AOS_RESULT AOS_API_TASK_REMOVE      (HANDLE hTask)                                                  { SAVE; asm("swi 2"); LOAD; }
inline AOS_RESULT AOS_API_TASK_SUSPEND     (HANDLE hTask)                                                  { SAVE; asm("swi 3"); LOAD; }
inline AOS_RESULT AOS_API_TASK_CONTINUE    (HANDLE hTask)                                                  { SAVE; asm("swi 4"); LOAD; }
inline AOS_RESULT AOS_API_TASK_GETHANDLE   (HANDLE* phTask)                                                { SAVE; asm("swi 5"); LOAD; }
inline AOS_RESULT AOS_API_CRITICAL_CREATE  (HANDLE* phCriticalSection)                                     { SAVE; asm("swi 6"); LOAD; }
inline AOS_RESULT AOS_API_CRITICAL_DELETE  (HANDLE hCriticalSection)                                       { SAVE; asm("swi 7"); LOAD; }
inline AOS_RESULT AOS_API_CRITICAL_ENTER   (HANDLE hCriticalSection)                                       { SAVE; asm("swi 8"); LOAD; }
inline AOS_RESULT AOS_API_CRITICAL_LEAVE   (HANDLE hCriticalSection)                                       { SAVE; asm("swi 9"); LOAD; }

inline AOS_RESULT AOS_API_ASSERT           (const char* pszReason)                                         { SAVE; asm("swi 10"); LOAD; }
inline AOS_RESULT AOS_API_SLEEP            (unsigned long nMilliseconds)                                   { SAVE; asm("swi 11"); LOAD; }
//#define nAOS_API_GET_OS_VER      12
//#define nAOS_API_GETLASTERROR    13
//#define nAOS_API_GETSYSTEMVAL    14
//#define nAOS_API_SETSYSTEMVAL    15
inline AOS_RESULT AOS_API_POWERSAVE        (unsigned long bSave)                                           { SAVE; asm("swi 16"); LOAD; }
inline AOS_RESULT AOS_API_SHUTDOWN         ()                                                              { SAVE; asm("swi 17"); LOAD; }

inline AOS_RESULT AOS_API_GETTIME          (AOS_TIME* pTime)                                               { SAVE; asm("swi 20"); LOAD; }
inline AOS_RESULT AOS_API_GETTICKCOUNT     (unsigned long* pnTicks)                                        { SAVE; asm("swi 21"); LOAD; }
inline AOS_RESULT AOS_API_GETKEYSTATE      (unsigned long* pKey)                                           { SAVE; asm("swi 22"); LOAD; }
inline AOS_RESULT AOS_API_TEXTOUT          (int nX, int nY, const char* pszText)                           { SAVE; asm("swi 23"); LOAD; }
inline AOS_RESULT AOS_API_READKEY          (unsigned char* pKey)                                           { SAVE; asm("swi 24"); LOAD; }

inline AOS_RESULT AOS_API_FPU_ADD          (float* pSrc1, float* pSrc2, int nCount)                        { SAVE; asm("swi 25"); LOAD; }
inline AOS_RESULT AOS_API_FPU_SUB          (float* pSrc1, float* pSrc2, int nCount)                        { SAVE; asm("swi 26"); LOAD; }
inline AOS_RESULT AOS_API_FPU_MUL          (float* pSrc1, float* pSrc2, int nCount)                        { SAVE; asm("swi 27"); LOAD; }
inline AOS_RESULT AOS_API_FPU_DIV          (float* pSrc1, float* pSrc2, int nCount)                        { SAVE; asm("swi 28"); LOAD; }
inline AOS_RESULT AOS_API_FPU_CNV3D        (float* pDst, AOS_3DVERTEX_BUFFER* pVertices, float* pMatrix)   { SAVE; asm("swi 29"); LOAD; }

inline AOS_RESULT AOS_API_MALLOC           (void** ppvBuffer, unsigned long nBytes)                        { SAVE; asm("swi 30"); LOAD; }
inline AOS_RESULT AOS_API_FREE             (void* pvBuffer)                                                { SAVE; asm("swi 31"); LOAD; }
inline AOS_RESULT AOS_API_MEMAVAIL         (unsigned long* pnBytes)                                        { SAVE; asm("swi 32"); LOAD; }
//#define nAOS_API_MEMSET          33
//#define nAOS_API_MEMCPY          34

inline AOS_RESULT AOS_API_PLAYSOUND        (void* pData, int nSize)                                        { SAVE; asm("swi 40"); LOAD; }
inline AOS_RESULT AOS_API_ISPLAYING        (int* nResult)                                                  { SAVE; asm("swi 41"); LOAD; }
inline AOS_RESULT AOS_API_STOPSOUND        ()                                                              { SAVE; asm("swi 42"); LOAD; }
inline AOS_RESULT AOS_API_GETSOUNDPARAM    (int nParamID, int* pValue)                                     { SAVE; asm("swi 43"); LOAD; }
inline AOS_RESULT AOS_API_SETSOUNDPARAM    (int nParamID, int nValue)                                      { SAVE; asm("swi 44"); LOAD; }
inline AOS_RESULT AOS_API_UART0_RECV       (unsigned char* pSymbol)                                        { SAVE; asm("swi 45"); LOAD; }
inline AOS_RESULT AOS_API_UART0_PIPE       (HANDLE* phPipe)                                                { SAVE; asm("swi 46"); LOAD; }
inline AOS_RESULT AOS_API_BYTESPLAYED      (unsigned long* pnBytes)                                        { SAVE; asm("swi 47"); LOAD; }

inline AOS_RESULT AOS_API_SECTOR_READ      (unsigned long lba, unsigned long nCnt, unsigned char* pBuffer) { SAVE; asm("swi 50"); LOAD; }
inline AOS_RESULT AOS_API_SECTOR_WRITE     (unsigned long lba, unsigned long nCnt, unsigned char* pBuffer) { SAVE; asm("swi 51"); LOAD; }
//#define nAOS_API_DRIVES_ENUM     52
inline AOS_RESULT AOS_API_FINDFIRST_FILE   (const char* pszPath, FILE_FIND* pSearchHandle)                 { SAVE; asm("swi 53"); LOAD; }
inline AOS_RESULT AOS_API_FINDNEXT_FILE    (FILE_FIND* pSearchHandle, char* pszFileName)                   { SAVE; asm("swi 54"); LOAD; }
inline AOS_RESULT AOS_API_FILE_OPEN        (const char* pszFilename, FILE_INFO* pFileHandle)               { SAVE; asm("swi 55"); LOAD; }
inline AOS_RESULT AOS_API_FILE_READ        (FILE_INFO* pFileHandle, void* pBuffer, unsigned long nBytes)   { SAVE; asm("swi 56"); LOAD; }
inline AOS_RESULT AOS_API_FILE_WRITE       (FILE_INFO* pFileHandle, void* pBuffer, unsigned long nBytes)   { SAVE; asm("swi 57"); LOAD; }

//#define nAOS_API_LINE            60
inline AOS_RESULT AOS_API_RECT             (IMAGE* pImage, RECT* pRect, COLOR Color)                       { SAVE; asm("swi 61"); LOAD; }
inline AOS_RESULT AOS_API_FILLRECT         (IMAGE* pImage, RECT* pRect, COLOR Color)                       { SAVE; asm("swi 62"); LOAD; }
inline AOS_RESULT AOS_API_SETBKCOLOR       (unsigned long nColor)                                          { SAVE; asm("swi 63"); LOAD; }
inline AOS_RESULT AOS_API_SETTEXTCOLOR     (unsigned long nColor)                                          { SAVE; asm("swi 64"); LOAD; }
inline AOS_RESULT AOS_API_SETFONT          (void* pFont)                                                   { SAVE; asm("swi 65"); LOAD; }
inline AOS_RESULT AOS_API_BITBLT           (IMAGE* pDst, IMAGE* pSrc, POINT* pOrigin)                      { SAVE; asm("swi 66"); LOAD; }
inline AOS_RESULT AOS_API_STRETCHBLT       (IMAGE* pDst, IMAGE* pSrc, unsigned long* pBuffer)              { SAVE; asm("swi 67"); LOAD; }
//inline void AOS_API_TRANSPBLT        ()                                                              { SAVE; asm("swi 68"); LOAD; }
inline AOS_RESULT AOS_API_PATBLT           (IMAGE* pDst, IMAGE* pSrc)                                      { SAVE; asm("swi 69"); LOAD; }

inline AOS_RESULT AOS_API_IMAGE_CREATE     (RECT* pRect, IMAGE** pImage)                                   { SAVE; asm("swi 70"); LOAD; }
inline AOS_RESULT AOS_API_IMAGE_DELETE     (IMAGE* pImage)                                                 { SAVE; asm("swi 71"); LOAD; }
inline AOS_RESULT AOS_API_GET_TASK_DC      (IMAGE* pImage)                                                 { SAVE; asm("swi 72"); LOAD; }
inline AOS_RESULT AOS_API_SET_TASK_DC      (RECT* pRect, int nLayer)                                       { SAVE; asm("swi 73"); LOAD; }
inline AOS_RESULT AOS_API_GET_DC_PARAMS    (IMAGE* pImage)                                                 { SAVE; asm("swi 74"); LOAD; }
inline AOS_RESULT AOS_API_SET_DC_PARAMS    ()                                                              { SAVE; asm("swi 75"); LOAD; }
inline AOS_RESULT AOS_API_IMAGE_DC         (RECT* pRect, int nLayer, IMAGE* pImage)                        { SAVE; asm("swi 76"); LOAD; }
inline AOS_RESULT AOS_API_RENDER           (AOS_3DDESC* pDesc, AOS_3DMODEL* pModel)                        { SAVE; asm("swi 77"); LOAD; }

inline AOS_RESULT AOS_API_PIPE_CREATE      (HANDLE* phPipe)                                                { SAVE; asm("swi 80"); LOAD; }
inline AOS_RESULT AOS_API_PIPE_DELETE      (HANDLE hPipe)                                                  { SAVE; asm("swi 81"); LOAD; }
inline AOS_RESULT AOS_API_PIPE_SEND        (HANDLE hPipe, void* pData, unsigned long nBytesToSend)         { SAVE; asm("swi 82"); LOAD; }
inline AOS_RESULT AOS_API_PIPE_RECV        (HANDLE hPipe, void* pData, unsigned long nBytesToReceive)      { SAVE; asm("swi 83"); LOAD; }
*/

#endif
