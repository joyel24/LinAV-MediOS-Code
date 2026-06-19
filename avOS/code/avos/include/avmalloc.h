//    Interface definitions for avmalloc.c (aka BGET), the memory management package.


//**** (GCL 3/19/04) ************************************************************************
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

SEE mallocTest.c in avos /src/examples directory for a sample usage and test program.
It demonstrates the actual code necessary.
*/
//*******************************************************************************************


#ifndef _AVMALLOC_H_
#define _AVMALLOC_H_ 1

#include <stdio.h>

#ifndef _
#ifdef PROTOTYPES
#define  _(x)  x		      /* If compiler knows prototypes */
#else
#define  _(x)  ()                     /* If it doesn't */
#endif /* PROTOTYPES */
#endif


typedef long bufsize;

void	bpool	    _((void *buffer, bufsize len));
void   *bget	    _((bufsize size));
void   *bgetz	    _((bufsize size));
void   *bgetr	    _((void *buffer, bufsize newsize));
void	brel	    _((void *buf));

//**** (GCL 3/19/04) ****************Comment out all the extras we don't need now  **********

//void	bectl	    _((int (*compact)(bufsize sizereq, int sequence),
//		       void *(*acquire)(bufsize size),
//		       void (*release)(void *buf), bufsize pool_incr));
//void	bstats	    _((bufsize *curalloc, bufsize *totfree, bufsize *maxfree,
//		       long *nget, long *nrel));
//void	bstatse     _((bufsize *pool_incr, long *npool, long *npget,
//		       long *nprel, long *ndget, long *ndrel));
//void	bufdump     _((void *buf));
//void	bpoold	    _((void *pool, int dumpalloc, int dumpfree));
//int	bpoolv	    _((void *pool));

#endif
