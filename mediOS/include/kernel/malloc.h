#ifndef __MALLOC_H
#define __MALLOC_H


#define memstat(pool)            bpoold(pool,0,0)
#define malloc_init(beg,size)    {bpool(beg,size);printk("[init] malloc\n");}

typedef long bufsize;


void  mem_addPool  (void *buffer, long len);
void* malloc       (long size);
void* realloc      (void *buffer, long newsize);
void  free         (void *buf);
void  mem_dump     (void *pool, int dumpalloc, int dumpfree);
void  mem_stat     (long *curalloc, long *totfree, long *maxfree,long * nget, long *nrel);
#ifdef HAS_bgetz
void* bgetz        (long size);
#endif

/*

PROVIDES:  equivalent functions to malloc, calloc, realloc, and free

USAGE (GENERAL OVERVIEW):

    1)  Initialize with bpool(addr, size) where addr = start address of area to use

                                          and where size = total size in bytes of area to use

        -- sets aside a block of linear mem that will be used for future allocation requests

        -- you can call this function later again to add more chunks to total

        -- up to you to avoid conflicts with anything else, recommend using top of mem?



    2a) Use bget(size) where size is desired size of memory block in bytes

                       This is the substitute for MALLOC

                       Returns pointer to block of memory, or '0' if not possible or error



    2b) Use brel(ptr) where ptr is pointer to previously allocated memory block

                      This is the substitute for FREE 

                      Will release a previously allocated block of memory.

    2c) Use bgetz(size) where size is desired size of mem block in bytes, like for bget/malloc

                        This is the substitute for CALLOC

                        Returns pointer to block of memory, or '0' if not possible or error

                        And also initializes the allocated block with all zero's.

    2d) Use bgetr(ptr, size) where ptr is pointer to previously allocated block and

                             where size is the new desired size for this block

                             This is the substitute for REALLOC

                             This will expand the size of a previously allocated block

                                and will return the new address.

                             Contents of original block are retained (with a slow copying

                                function here, avoid using for max speed)

                             If new size is smaller, fine, but some orig content will be lost

                                obviously...


*/

#endif

