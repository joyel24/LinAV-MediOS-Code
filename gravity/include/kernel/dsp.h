/* 
*   kernel/dsp.h
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __DSP_H
#define __DSP_H

#define HPIBCTL  0x00030600
#define MOD1_REG 0x00030894
#define MOD2_REG 0x00030896

// Common DSP control methods
void dsp_on ();
void dsp_off ();
void dsp_clear ();
void dsp_reset ();
void dsp_run ();

// Data structures to use with TI CodeComposerStudio executable files
typedef struct _COFF_FILHDR
{
	unsigned short f_magic;         /* magic number             */
	unsigned short f_nscns;         /* number of sections       */
	unsigned long  f_timdat;        /* time & date stamp        */
	unsigned long  f_symptr;        /* file pointer to symtab   */
	unsigned long  f_nsyms;         /* number of symtab entries */
	unsigned short f_opthdr;        /* sizeof(optional hdr)     */
	unsigned short f_flags;         /* flags                    */
//0x0001 F_RELFLG If set, there is no relocation information in this file. This is usually clear for objects and set for executables. 
//0x0002 F_EXEC If set, all unresolved symbols have been resolved and the file may be considered executable. 
//0x0004 F_LNNO If set, all line number information has been removed from the file (or was never added in the first place). 
//0x0008 F_LSYMS If set, all the local symbols have been removed from the file (or were never added in the first place). 
//0x0100 F_AR32WR Indicates that the file is 32-bit little endian 
} COFF_FILHDR;

typedef struct _COFF_SCNHDR
{
	char           s_name[8];  /* section name                     */
	unsigned short s_paddr_lo;    /* physical address, aliased s_nlib */
	unsigned short s_paddr_hi;    /* physical address, aliased s_nlib */
	unsigned short s_vaddr_lo;    /* virtual address                  */
	unsigned short s_vaddr_hi;    /* virtual address                  */
	unsigned short s_size_lo;     /* section size                     */
	unsigned short s_size_hi;     /* section size                     */
	unsigned short s_scnptr_lo;   /* file ptr to raw data for section */
	unsigned short s_scnptr_hi;   /* file ptr to raw data for section */
	unsigned long  s_relptr;   /* file ptr to relocation           */
	unsigned long  s_lnnoptr;  /* file ptr to line numbers         */
	unsigned short s_nreloc;   /* number of relocation entries     */
	unsigned short s_nlnno;    /* number of line number entries    */
	unsigned long  s_flags;    /* flags                            */
//0x0020 STYP_TEXT If set, indicates that this section contains only executable code. 
//0x0040 STYP_DATA If set, indicates that this section contains only initialized data. 
//0x0080 STYP_BSS If set, indicates that this section defines uninitialized data, and has no data stored in the coff file for it. 
} COFF_SCNHDR;

void load_dsp_program (const char* pszFilename);



////////////////////////////////////////////////////////
// DSP - AV3XX interconnection data structs,
// located in shared memory at address 0x0004100.
////////////////////////////////////////////////////////
typedef struct _AV3XX_DSP_GFX_CONTEXT
{
	unsigned long  pPixels;
	unsigned short nWidth;
	unsigned short nHeight;
	unsigned short nStride;
} AV3XX_DSP_GFX_CONTEXT;

typedef struct _AV3XX_DSP_INTERCONNECTION
{
	unsigned short        nCommandToDSP;
	unsigned short        nDSPResultCode;
	unsigned short        nCommandToAV3XX;
	unsigned short        nAV3XXResultCode;
	unsigned long         nMemoryFileStart;
	unsigned long         nMemoryFileSize;
	unsigned long         nDecodingBufferAddress;
	AV3XX_DSP_GFX_CONTEXT ctx;
	short                 sMsgBuffer [64];
} AV3XX_DSP_INTERCONNECTION;

#define DSP_TO_AV3XX_COMMAND_MESSAGE  0x0100
#define DSP_TO_AV3XX_COMMAND_READFILE 0x0101
#define DSP_TO_AV3XX_COMMAND_COMPLETE 0x0102
////////////////////////////////////////////////////////



#endif //__DSP_H
