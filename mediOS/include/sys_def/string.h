/* 
*   include/sys_def/stddef.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
*/

#ifndef __SYS_DEF_STRING_H
#define __SYS_DEF_STRING_H

#include <stdarg.h>

#include <sys_def/types.h>	/* for size_t */
#include <sys_def/stddef.h>	/* for NULL */


extern char * ___strtok;

#define    toLower(chr)  ((chr>64 && chr<91)?chr+32:chr)

void strlwr(char *s);

void    qsort       (void* __base, size_t __nmemb, size_t __size, int(*_compar)(const void*, const void*));

int     atoi        (const char *str);

char *  strdup      (const char * s);
int     strcasecmp  (const char *s1, const char *s2);
int     strncasecmp (const char *s1, const char *s2, size_t n);
char *  strpbrk     (const char *,const char *);
char *  strtok      (char *,const char *);
char *  strtok_r    (char *ptr, const char *sep, char **end);
char *  strsep      (char **,const char *);
size_t  strspn      (const char *,const char *);
char *  strcpy      (char *,const char *);
char *  strncpy     (char *,const char *, size_t);
char *  strcat      (char *, const char *);
char *  strncat     (char *, const char *, size_t);
int     strcmp      (const char *,const char *);
int     strncmp     (const char *,const char *,size_t);
int     strnicmp    (const char *, const char *, size_t);
char *  strchr      (const char *,int);
char *  strrchr     (const char *,int);
char *  strstr      (const char *,const char *);
size_t  strlen      (const char *);
size_t  strnlen     (const char *,size_t);
void *  memset      (void *,int,size_t);
void *  memcpy      (void *,const void *,size_t);
void *  memmove     (void *,const void *,size_t);
void *  memscan     (void *,int,size_t);
int     memcmp      (const void *,const void *,size_t);
void *  memchr      (const void *,int,size_t);

int     vsnprintf   (char *buf, size_t size, const char *fmt, va_list args);
int     snprintf    (char * buf, size_t size, const char *fmt, ...);
int     vsprintf    (char *buf, const char *fmt, va_list args);
int     sprintf     (char * buf, const char *fmt, ...);
int     vsscanf     (const char * buf, const char * fmt, va_list args);
int     sscanf      (const char * buf, const char * fmt, ...);
#endif 
