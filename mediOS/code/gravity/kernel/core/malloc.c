/////////////////////////////////
//malloc.c

#include <kernel/malloc.h>

#define NDEBUG
#include <assert.h>

//#include <memory.h>



#ifdef BufDump			      /* BufDump implies DumpData */
    #ifndef DumpData
        #define DumpData    1
    #endif
#endif

#ifdef DumpData
#include <sys_def/ctype.h>
#endif



/*  Declare the interface, including the requested buffer size type,

    bufsize.  */


#include <sys_def/string.h>
#include <kernel/malloc.h>
#include <kernel/kernel.h>




/*  Minimum allocation quantum: */

#define QLSize	(sizeof(struct qlinks))

#define SizeQ	((SizeQuant > QLSize) ? SizeQuant : QLSize)



#define V   (void)		      /* To denote unwanted returned values */



/* End sentinel: value placed in bsize field of dummy block delimiting

   end of pool block.  The most negative number which will  fit  in  a

   bufsize, defined in a way that the compiler will accept. */



#define ESent	((bufsize) (-(((1L << (sizeof(bufsize) * 8 - 2)) - 1) * 2) - 2))



/*  BGET  --  Allocate a buffer.  */



__IRAM_CODE void *bget(MEMORY_CONTEXT* pContext, bufsize requested_size)
{
    bufsize size = requested_size;


    struct bfhead *b;

#ifdef BestFit

    struct bfhead *best;

#endif

    void *buf;

#ifdef BECtl

    int compactseq = 0;

#endif



    //printk("[malloc] request: %d\n",size);


    assert(size > 0);



    if (size < SizeQ) { 	      /* Need at least room for the */

	size = SizeQ;		      /*    queue links.  */

    }

#ifdef SizeQuant

#if SizeQuant > 1

    size = (size + (SizeQuant - 1)) & (~(SizeQuant - 1));

#endif

#endif



    size += sizeof(struct bhead);     /* Add overhead in allocated buffer

					 to size required. */

    //printk("computed: %d \n",size);
    //bpoold((void*)0x03100000,0,0);

#ifdef BECtl

    /* If a compact function was provided in the call to bectl(), wrap

       a loop around the allocation process  to  allow	compaction  to

       intervene in case we don't find a suitable buffer in the chain. */



    while (1) {

#endif

	b = pContext->freelist.ql.flink;

#ifdef BestFit

	best = &pContext->freelist;

#endif





	/* Scan the free list searching for the first buffer big enough

	   to hold the requested size buffer. */



#ifdef BestFit

	while (b != &pContext->freelist) {

	    if (b->bh.bsize >= size) {

		if ((best == &pContext->freelist) || (b->bh.bsize < best->bh.bsize)) {

		    best = b;

		}

	    }

	    b = b->ql.flink;		  /* Link to next buffer */

	}

	b = best;

#endif /* BestFit */



	while (b != &pContext->freelist) {
//printk("buf %08x size: %d \n",b,b->bh.bsize);
	    if ((bufsize) b->bh.bsize >= size) {

//printk("buff found");

		/* Buffer  is big enough to satisfy  the request.  Allocate it

		   to the caller.  We must decide whether the buffer is  large

		   enough  to  split  into  the part given to the caller and a

		   free buffer that remains on the free list, or  whether  the

		   entire  buffer  should  be  removed	from the free list and

		   given to the caller in its entirety.   We  only  split  the

		   buffer if enough room remains for a header plus the minimum

		   quantum of allocation. */



		if ((b->bh.bsize - size) > (SizeQ + (sizeof(struct bhead)))) {

		    struct bhead *ba, *bn;

//printk(" splitting");

		    ba = BH(((char *) b) + (b->bh.bsize - size));

		    bn = BH(((char *) ba) + size);

		    assert(bn->prevfree == b->bh.bsize);

		    /* Subtract size from length of free block. */

		    b->bh.bsize -= size;

		    /* Link allocated buffer to the previous free buffer. */

		    ba->prevfree = b->bh.bsize;

		    /* Plug negative size into user buffer. */

		    ba->bsize = -(bufsize) size;

		    /* Mark buffer after this one not preceded by free block. */

		    bn->prevfree = 0;



#ifdef BufStats

		    pContext->totalloc += size;

		    pContext->numget++;		  /* Increment number of bget() calls */

#endif

		    buf = (void *) ((((char *) ba) + sizeof(struct bhead)));
//printk(" %08x\n",buf);
		    return buf;

		} else {

		    struct bhead *ba;

//printk(" no split");

		    ba = BH(((char *) b) + b->bh.bsize);

		    assert(ba->prevfree == b->bh.bsize);



                    /* The buffer isn't big enough to split.  Give  the  whole

		       shebang to the caller and remove it from the free list. */



		    assert(b->ql.blink->ql.flink == b);

		    assert(b->ql.flink->ql.blink == b);

		    b->ql.blink->ql.flink = b->ql.flink;

		    b->ql.flink->ql.blink = b->ql.blink;



#ifdef BufStats

		    pContext->totalloc += b->bh.bsize;

		    pContext->numget++;		  /* Increment number of bget() calls */

#endif

		    /* Negate size to mark buffer allocated. */

		    b->bh.bsize = -(b->bh.bsize);



		    /* Zero the back pointer in the next buffer in memory

		       to indicate that this buffer is allocated. */

		    ba->prevfree = 0;



		    /* Give user buffer starting at queue links. */

		    buf =  (void *) &(b->ql);
//printk(" %08x\n",buf);
		    return buf;

		}

	    }
            
	    b = b->ql.flink;		  /* Link to next buffer */

	}

#ifdef BECtl



        /* We failed to find a buffer.  If there's a compact  function

	   defined,  notify  it  of the size requested.  If it returns

	   TRUE, try the allocation again. */



	if ((pContext->compfcn == NULL) || (!(*pContext->compfcn)(size, ++compactseq))) {
	    break;

	}

    }



    /* No buffer available with requested size free. */



    /* Don't give up yet -- look in the reserve supply. */
    


    if (acqfcn != NULL) {

	if (size > exp_incr - sizeof(struct bhead)) {



	    /* Request	is  too  large	to  fit in a single expansion

	       block.  Try to satisy it by a direct buffer acquisition. */



	    struct bdhead *bdh;



	    size += sizeof(struct bdhead) - sizeof(struct bhead);

	    if ((bdh = BDH((*pContext->acqfcn)((bufsize) size))) != NULL) {



		/*  Mark the buffer special by setting the size field

		    of its header to zero.  */

		bdh->bh.bsize = 0;

		bdh->bh.prevfree = 0;

		bdh->tsize = size;

#ifdef BufStats

		pContext->totalloc += size;

		pContext->numget++;	      /* Increment number of bget() calls */

		pContext->numdget++;	      /* Direct bget() call count */

#endif

		buf =  (void *) (bdh + 1);

		return buf;

	    }



	} else {



	    /*	Try to obtain a new expansion block */



	    void *newpool;



	    if ((newpool = (*pContext->acqfcn)((bufsize) exp_incr)) != NULL) {

		bpool(pContext, newpool, exp_incr);

                buf =  bget(pContext, requested_size);  /* This can't, I say, can't

						 get into a loop. */

		return buf;

	    }

	}

    }



    /*	Still no buffer available */



#endif /* BECtl */

//printk("buff not found ");

    return NULL;

}








/*  BREL  --  Release a buffer.  */



__IRAM_CODE void brel(MEMORY_CONTEXT* pContext, void *buf)
{

    struct bfhead *b, *bn;

    //printk("release %08x\n",buf);

    b = BFH(((char *) buf) - sizeof(struct bhead));

#ifdef BufStats

    pContext->numrel++;			      /* Increment number of brel() calls */

#endif

    assert(buf != NULL);



#ifdef BECtl

    if (b->bh.bsize == 0) {	      /* Directly-acquired buffer? */

	struct bdhead *bdh;



	bdh = BDH(((char *) buf) - sizeof(struct bdhead));

	assert(b->bh.prevfree == 0);

#ifdef BufStats

	pContext->totalloc -= bdh->tsize;

	assert(pContext->totalloc >= 0);

	pContext->numdrel++;		      /* Number of direct releases */

#endif /* BufStats */

#ifdef FreeWipe

	V memset((char *) buf, 0x55,

		 (MemSize) (bdh->tsize - sizeof(struct bdhead)));

#endif /* FreeWipe */

	assert(pContext->relfcn != NULL);

	(*pContext->relfcn)((void *) bdh);      /* Release it directly. */

	return;

    }

#endif /* BECtl */



    /* Buffer size must be negative, indicating that the buffer is

       allocated. */



    if (b->bh.bsize >= 0) {

	bn = NULL;

    }

    assert(b->bh.bsize < 0);



    /*	Back pointer in next buffer must be zero, indicating the

	same thing: */



    assert(BH((char *) b - b->bh.bsize)->prevfree == 0);



#ifdef BufStats

    pContext->totalloc += b->bh.bsize;

    assert(pContext->totalloc >= 0);

#endif



    /* If the back link is nonzero, the previous buffer is free.  */



    if (b->bh.prevfree != 0) {



	/* The previous buffer is free.  Consolidate this buffer  with	it

	   by  adding  the  length  of	this  buffer  to the previous free

	   buffer.  Note that we subtract the size  in	the  buffer  being

           released,  since  it's  negative to indicate that the buffer is

	   allocated. */



	register bufsize size = b->bh.bsize;



        /* Make the previous buffer the one we're working on. */

	assert(BH((char *) b - b->bh.prevfree)->bsize == b->bh.prevfree);

	b = BFH(((char *) b) - b->bh.prevfree);

	b->bh.bsize -= size;

    } else {



        /* The previous buffer isn't allocated.  Insert this buffer

	   on the free list as an isolated free block. */



	assert(pContext->freelist.ql.blink->ql.flink == &pContext->freelist);

	assert(pContext->freelist.ql.flink->ql.blink == &pContext->freelist);

	b->ql.flink = &pContext->freelist;

	b->ql.blink = pContext->freelist.ql.blink;

	pContext->freelist.ql.blink = b;

	b->ql.blink->ql.flink = b;

	b->bh.bsize = -b->bh.bsize;

    }



    /* Now we look at the next buffer in memory, located by advancing from

       the  start  of  this  buffer  by its size, to see if that buffer is

       free.  If it is, we combine  this  buffer  with	the  next  one	in

       memory, dechaining the second buffer from the free list. */



    bn =  BFH(((char *) b) + b->bh.bsize);

    if (bn->bh.bsize > 0) {



	/* The buffer is free.	Remove it from the free list and add

	   its size to that of our buffer. */



	assert(BH((char *) bn + bn->bh.bsize)->prevfree == bn->bh.bsize);

	assert(bn->ql.blink->ql.flink == bn);

	assert(bn->ql.flink->ql.blink == bn);

	bn->ql.blink->ql.flink = bn->ql.flink;

	bn->ql.flink->ql.blink = bn->ql.blink;

	b->bh.bsize += bn->bh.bsize;



	/* Finally,  advance  to   the	buffer	that   follows	the  newly

	   consolidated free block.  We must set its  backpointer  to  the

	   head  of  the  consolidated free block.  We know the next block

	   must be an allocated block because the process of recombination

	   guarantees  that  two  free	blocks will never be contiguous in

	   memory.  */



	bn = BFH(((char *) b) + b->bh.bsize);

    }

#ifdef FreeWipe

    V memset(((char *) b) + sizeof(struct bfhead), 0x55,

	    (MemSize) (b->bh.bsize - sizeof(struct bfhead)));

#endif

    assert(bn->bh.bsize < 0);



    /* The next buffer is allocated.  Set the backpointer in it  to  point

       to this buffer; the previous free buffer in memory. */



    bn->bh.prevfree = b->bh.bsize;



#ifdef BECtl



    /*	If  a  block-release function is defined, and this free buffer

	constitutes the entire block, release it.  Note that  pool_len

	is  defined  in  such a way that the test will fail unless all

	pool blocks are the same size.	*/



    if (relfcn != NULL &&

	((bufsize) b->bh.bsize) == (pool_len - sizeof(struct bhead))) {



	assert(b->bh.prevfree == 0);

	assert(BH((char *) b + b->bh.bsize)->bsize == ESent);

	assert(BH((char *) b + b->bh.bsize)->prevfree == b->bh.bsize);

	/*  Unlink the buffer from the free list  */

	b->ql.blink->ql.flink = b->ql.flink;

	b->ql.flink->ql.blink = b->ql.blink;



	(*relfcn)(b);

#ifdef BufStats

	pContext->numprel++;		      /* Nr of expansion block releases */

	pContext->numpblk--;		      /* Total number of blocks */

	assert(pContext->numpblk == pContext->numpget - pContext->numprel);

#endif /* BufStats */

    }

#endif /* BECtl */

}



#ifdef BECtl



/*  BECTL  --  Establish automatic pool expansion control  */



void bectl(int (*compact) (bufsize sizereq, int sequence),
              void *(*acquire) (bufsize size),
              void (*release) (void *buf),
              bufsize pool_incr)
{

    pContext->compfcn = compact;

    pContext->acqfcn = acquire;

    pContext->relfcn = release;

    pContext->exp_incr = pool_incr;

}

#endif



/*  BPOOL  --  Add a region of memory to the buffer pool.  */



void bpool(MEMORY_CONTEXT* pContext, void *buf, bufsize len)
{

    struct bfhead *b = BFH(buf);

    struct bhead *bn;



#ifdef SizeQuant

    len &= ~(SizeQuant - 1);

#endif

#ifdef BECtl

    if (pContext->pool_len == 0) {

	pContext->pool_len = len;

    } else if (len != pContext->pool_len) {

	pContext->pool_len = -1;

    }

#ifdef BufStats

    pContext->numpget++;			      /* Number of block acquisitions */

    pContext->numpblk++;			      /* Number of blocks total */

    assert(pContext->numpblk == pContext->numpget - pContext->numprel);

#endif /* BufStats */

#endif /* BECtl */



    /* Since the block is initially occupied by a single free  buffer,

       it  had	better	not  be  (much) larger than the largest buffer

       whose size we can store in bhead.bsize. */



    assert(len - sizeof(struct bhead) <= -((bufsize) ESent + 1));



    /* Clear  the  backpointer at  the start of the block to indicate that

       there  is  no  free  block  prior  to  this   one.    That   blocks

       recombination when the first block in memory is released. */



    b->bh.prevfree = 0;



    /* Chain the new block to the free list. */



    assert(pContext->freelist.ql.blink->ql.flink == &pContext->freelist);

    assert(pContext->freelist.ql.flink->ql.blink == &pContext->freelist);

    b->ql.flink = &pContext->freelist;

    b->ql.blink = pContext->freelist.ql.blink;

    pContext->freelist.ql.blink = b;

    b->ql.blink->ql.flink = b;



    /* Create a dummy allocated buffer at the end of the pool.	This dummy

       buffer is seen when a buffer at the end of the pool is released and

       blocks  recombination  of  the last buffer with the dummy buffer at

       the end.  The length in the dummy buffer  is  set  to  the  largest

       negative  number  to  denote  the  end  of  the pool for diagnostic

       routines (this specific value is  not  counted  on  by  the  actual

       allocation and release functions). */



    len -= sizeof(struct bhead);

    b->bh.bsize = (bufsize) len;

#ifdef FreeWipe

    V memset(((char *) b) + sizeof(struct bfhead), 0x55,

	     (MemSize) (len - sizeof(struct bfhead)));

#endif

    bn = BH(((char *) b) + len);

    bn->prevfree = (bufsize) len;

    /* Definition of ESent assumes two's complement! */

    assert((~0) == -1);

    bn->bsize = ESent;

}



#ifdef BufStats



/*  BSTATS  --	Return buffer allocation free space statistics.  */



void bstats (MEMORY_CONTEXT* pContext, bufsize *curalloc, bufsize *totfree, bufsize *maxfree,long * nget, long *nrel)
{

    struct bfhead *b = pContext->freelist.ql.flink;



    *nget = pContext->numget;

    *nrel = pContext->numrel;

    *curalloc = pContext->totalloc;

    *totfree = 0;

    *maxfree = -1;

    while (b != &pContext->freelist) {

	assert(b->bh.bsize > 0);

	*totfree += b->bh.bsize;

	if (b->bh.bsize > *maxfree) {

	    *maxfree = b->bh.bsize;

	}

	b = b->ql.flink;	      /* Link to next buffer */

    }

}



#ifdef BECtl



/*  BSTATSE  --  Return extended statistics  */



void bstatse(MEMORY_CONTEXT* pContext, bufsize *pool_incr, long *npool, long *npget,long * nprel, long *ndget,long * ndrel)
{

    *pool_incr = (pContext->pool_len < 0) ? -exp_incr : exp_incr;

    *npool = pContext->numpblk;

    *npget = pContext->numpget;

    *nprel = pContext->numprel;

    *ndget = pContext->numdget;

    *ndrel = pContext->numdrel;

}

#endif /* BECtl */

#endif /* BufStats */



#ifdef DumpData



/*  BUFDUMP  --  Dump the data in a buffer.  This is called with the  user

		 data pointer, and backs up to the buffer header.  It will

		 dump either a free block or an allocated one.	*/



void bufdump (void *buf)
{

    struct bfhead *b;

    unsigned char *bdump;

    bufsize bdlen;



    b = BFH(((char *) buf) - sizeof(struct bhead));

    assert(b->bh.bsize != 0);

    if (b->bh.bsize < 0) {

	bdump = (unsigned char *) buf;

	bdlen = (-b->bh.bsize) - sizeof(struct bhead);

    } else {

	bdump = (unsigned char *) (((char *) b) + sizeof(struct bfhead));

	bdlen = b->bh.bsize - sizeof(struct bfhead);

    }



    while (bdlen > 0) {

	int i, dupes = 0;

	bufsize l = bdlen;

	char bhex[50], bascii[20];



	if (l > 16) {

	    l = 16;

	}



	for (i = 0; i < l; i++) {

            sprintf(bhex + i * 3, "%02X ", bdump[i]);

            bascii[i] = isprint(bdump[i]) ? bdump[i] : ' ';

	}

	bascii[i] = 0;

        printk("%-48s   %s\n", bhex, bascii);

	bdump += l;

	bdlen -= l;

	while ((bdlen > 16) && (memcmp((char *) (bdump - 16),

				       (char *) bdump, 16) == 0)) {

	    dupes++;

	    bdump += 16;

	    bdlen -= 16;

	}

	if (dupes > 1) {

	    printk(

                "     (%d lines [%d bytes] identical to above line skipped)\n",

		dupes, dupes * 16);

	} else if (dupes == 1) {

	    bdump -= 16;

	    bdlen += 16;

	}

    }

}

#endif



#ifdef BufDump



/*  BPOOLD  --	Dump a buffer pool.  The buffer headers are always listed.

		If DUMPALLOC is nonzero, the contents of allocated buffers

		are  dumped.   If  DUMPFREE  is  nonzero,  free blocks are

		dumped as well.  If FreeWipe  checking	is  enabled,  free

		blocks	which  have  been clobbered will always be dumped. */



void bpoold(MEMORY_CONTEXT* pContext, void *buf, int dumpalloc, int dumpfree)
{

    struct bfhead *b = BFH(buf);



    while (b->bh.bsize != ESent) {

	bufsize bs = b->bh.bsize;



	if (bs < 0) {

	    bs = -bs;

            V printk("Allocated buffer: size %6ld bytes.\n", (long) bs);

	    if (dumpalloc) {

		bufdump((void *) (((char *) b) + sizeof(struct bhead)));

	    }

	} else {

            char *lerr = "";



	    assert(bs > 0);

	    if ((b->ql.blink->ql.flink != b) ||

		(b->ql.flink->ql.blink != b)) {

                lerr = "  (Bad free list links)";

	    }

            V printk("Free block:       size %6ld bytes.%s\n",

		(long) bs, lerr);

#ifdef FreeWipe

	    lerr = ((char *) b) + sizeof(struct bfhead);

	    if ((bs > sizeof(struct bfhead)) && ((*lerr != 0x55) ||

		(memcmp(lerr, lerr + 1,

		  (MemSize) (bs - (sizeof(struct bfhead) + 1))) != 0))) {

		V printf(

                    "(Contents of above free block have been overstored.)\n");

		bufdump((void *) (((char *) b) + sizeof(struct bhead)));

	    } else

#endif

	    if (dumpfree) {

		bufdump((void *) (((char *) b) + sizeof(struct bhead)));

	    }

	}

	b = BFH(((char *) b) + bs);

    }

}

#endif /* BufDump */



#ifdef BufValid



/*  BPOOLV  --  Validate a buffer pool.  If NDEBUG isn't defined,

		any error generates an assertion failure.  */



int bpoolv(MEMORY_CONTEXT* pContext, void *buf)
{

    struct bfhead *b = BFH(buf);



    while (b->bh.bsize != ESent) {

	bufsize bs = b->bh.bsize;



	if (bs < 0) {

	    bs = -bs;

	} else {

            char *lerr = "";



	    assert(bs > 0);

	    if (bs <= 0) {

		return 0;

	    }

	    if ((b->ql.blink->ql.flink != b) ||

		(b->ql.flink->ql.blink != b)) {

                V printf("Free block: size %6ld bytes.  (Bad free list links)\n",

		     (long) bs);

		assert(0);

		return 0;

	    }

#ifdef FreeWipe

	    lerr = ((char *) b) + sizeof(struct bfhead);

	    if ((bs > sizeof(struct bfhead)) && ((*lerr != 0x55) ||

		(memcmp(lerr, lerr + 1,

		  (MemSize) (bs - (sizeof(struct bfhead) + 1))) != 0))) {

		V printf(

                    "(Contents of above free block have been overstored.)\n");

		bufdump((void *) (((char *) b) + sizeof(struct bhead)));

		assert(0);

		return 0;

	    }

#endif

	}

	b = BFH(((char *) b) + bs);

    }

    return 1;

}

#endif /* BufValid */

void init_memory_context (MEMORY_CONTEXT* pContext)
{
	pContext->freelist.bh.prevfree = 0;
	pContext->freelist.bh.bsize    = 0;

	pContext->freelist.ql.flink = &pContext->freelist;
	pContext->freelist.ql.blink = &pContext->freelist;

#ifdef BufStats

	pContext->totalloc = 0;	      /* Total space currently allocated */

	pContext->numget = 0;
	pContext->numrel = 0;   /* Number of bget() and brel() calls */

#ifdef BECtl

	pContext->numpblk = 0;	      /* Number of pool blocks */

	pContext->numpget = 0;
	pContext->numprel = 0; /* Number of block gets and rels */

	pContext->numdget = 0;
	pContext->numdrel = 0; /* Number of direct gets and rels */

#endif /* BECtl */

#endif /* BufStats */

#ifdef BECtl

/* Automatic expansion block management functions */

	pContext->compfcn = NULL;

	pContext->acqfcn = NULL;

	pContext->relfcn = NULL;

	pContext->exp_incr = 0;	      /* Expansion block size */

	pContext->pool_len = 0;	      /* 0: no bpool calls have been made

					 -1: not all pool blocks are

					     the same size

					 >0: (common) block size for all

					     bpool calls made so far

				      */

#endif
}
