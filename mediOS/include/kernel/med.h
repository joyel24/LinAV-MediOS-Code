/* 
*   include/kernel/med.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __MED_H
#define __MED_H

#include <kernel/kernel.h>
#include <sys_def/types.h>

#define ELF32_R_SYM(i)	  ((i)>>8)
#define ELF32_R_TYPE(i)   ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))

typedef struct {
    unsigned char   e_ident[16];     /* File identification. */
    uint16_t        e_type;         /* File type. */
    uint16_t        e_machine;      /* Machine architecture. */
    uint32_t        e_version;      /* ELF format version. */
    uint32_t        e_entry;        /* Entry point. */
    uint32_t        e_phoff;        /* Program header file offset. */
    uint32_t        e_shoff;        /* Section header file offset. */
    uint32_t        e_flags;        /* Architecture-specific flags. */
    uint16_t        e_ehsize;       /* Size of ELF header in bytes. */
    uint16_t        e_phentsize;    /* Size of program header entry. */
    uint16_t        e_phnum;        /* Number of program header entries. */
    uint16_t        e_shentsize;    /* Size of section header entry. */
    uint16_t        e_shnum;        /* Number of section header entries. */
    uint16_t        e_shstrndx;     /* Section name strings section. */
} elf_hdr;

typedef struct {
    uint32_t      sh_name;        /* Section name (index into the
                                    section header string table). */
    uint32_t      sh_type;        /* Section type. */
    uint32_t      sh_flags;       /* Section flags. */
    uint32_t      sh_addr;        /* Address in memory image. */
    uint32_t      sh_offset;      /* Offset in file. */
    uint32_t      sh_size;        /* Size in bytes. */
    uint32_t      sh_link;        /* Index of a related section. */
    uint32_t      sh_info;        /* Depends on section type. */
    uint32_t      sh_addralign;   /* Alignment in bytes. */
    uint32_t      sh_entsize;     /* Size of each entry in section. */
} section_hdr;

typedef struct {
    uint32_t	r_offset;
    uint32_t	r_info;
} rel_entry;

#define MED_DISCARD         0
#define MED_BIT             1
#define MED_BSS             2
#define MED_REL             3

#define MED_DEST_IRAM       0
#define MED_DEST_SDRAM      1


typedef struct __section_t {
    char*           name;
    uint32_t        vaddr;
    char*           addr;
    uint32_t        offset;
    uint32_t        size;
    int             type;
    int             dest;
    int             opt;
    int             nb_ent;
    struct __section_t *   rel;
} section_t;

int med_load(char * file_name);
int med_loadParam(int argc,char**argv);

#endif
