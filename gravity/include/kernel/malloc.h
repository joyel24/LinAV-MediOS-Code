/////////////////////////////////
//malloc.h

#ifndef __MALLOC_H
#define __MALLOC_H

#define SizeQuant   4		      

#define BufDump     1	      
/*#define BufValid    1	*/	      
/*#define DumpData    1	*/	      
#define BufStats    1      
/*#define FreeWipe    1	*/	      
/*#define BestFit     1	*/	      
/*#define BECtl	    1	*/	      

typedef long bufsize;

#define MemSize     int

/* Queue links */



struct qlinks {

    struct bfhead *flink;	      /* Forward link */

    struct bfhead *blink;	      /* Backward link */

};



/* Header in allocated and free buffers */



struct bhead {

    bufsize prevfree;		      /* Relative link back to previous

					 free buffer in memory or 0 if

					 previous buffer is allocated.	*/

    bufsize bsize;		      /* Buffer size: positive if free,

					 negative if allocated. */

};

#define BH(p)	((struct bhead *) (p))


/*  Header in directly allocated buffers (by acqfcn) */



struct bdhead {

    bufsize tsize;		      /* Total size, including overhead */

    struct bhead bh;		      /* Common header */

};

#define BDH(p)	((struct bdhead *) (p))



/* Header in free buffers */



struct bfhead {

    struct bhead bh;		      /* Common allocated/free header */

    struct qlinks ql;		      /* Links on free list */

};

#define BFH(p)	((struct bfhead *) (p))


typedef struct _MEMORY_CONTEXT
{

	struct bfhead freelist;     /* List of free buffers */
//    {0, 0},
//    {&freelist, &freelist}


#ifdef BufStats

	bufsize totalloc;	      /* Total space currently allocated */

	long numget, numrel;   /* Number of bget() and brel() calls */

#ifdef BECtl

	long numpblk;	      /* Number of pool blocks */

	long numpget, numprel; /* Number of block gets and rels */

	long numdget, numdrel; /* Number of direct gets and rels */

#endif /* BECtl */

#endif /* BufStats */

#ifdef BECtl

/* Automatic expansion block management functions */

	int (*compfcn) _((bufsize sizereq, int sequence));

	void *(*acqfcn) _((bufsize size));

	void (*relfcn) _((void *buf));

	bufsize exp_incr;	      /* Expansion block size */

	bufsize pool_len;	      /* 0: no bpool calls have been made

					 -1: not all pool blocks are

					     the same size

					 >0: (common) block size for all

					     bpool calls made so far

				      */

#endif

} MEMORY_CONTEXT;

//#define memstat(pool)            bpoold(pool,0,0)
//#define init_malloc(beg,size)    {bpool(beg,size);printk("[init] malloc\n");}

void    init_memory_context (MEMORY_CONTEXT* pContext);
void    bpool  (MEMORY_CONTEXT* pContext, void *buffer, bufsize len);
void   *bget   (MEMORY_CONTEXT* pContext, bufsize size);
//void   *bgetz  (MEMORY_CONTEXT* pContext, bufsize size);
//void   *bgetr  (MEMORY_CONTEXT* pContext, void *buffer, bufsize newsize);
void    brel   (MEMORY_CONTEXT* pContext, void *buf);
void    bpoold (MEMORY_CONTEXT* pContext, void *pool, int dumpalloc, int dumpfree);
void    bstats (MEMORY_CONTEXT* pContext, bufsize *curalloc, bufsize *totfree, bufsize *maxfree,long * nget, long *nrel);

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

