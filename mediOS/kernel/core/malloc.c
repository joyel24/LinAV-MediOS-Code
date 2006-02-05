/* 
*   kernel/core/mallo.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* Original code from BGET
*
*/

/*  Declare the interface, including the requested buffer size type,bufsize.  */
#include <sys_def/ctype.h>
#include <sys_def/string.h>
#include <kernel/malloc.h>
#include <kernel/kernel.h>

#define MemSize     int 
#define SizeQuant   4

void bufdump(void *buf);

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

static struct bfhead freelist = {     /* List of free buffers */
    {0, 0},
    {&freelist, &freelist}
};

static bufsize totalloc = 0;	      /* Total space currently allocated */
static long numget = 0, numrel = 0;   /* Number of bget() and brel() calls */


/*  Minimum allocation quantum: */
#define QLSize	(sizeof(struct qlinks))
#define SizeQ	((SizeQuant > QLSize) ? SizeQuant : QLSize)


/* End sentinel: value placed in bsize field of dummy block delimiting
   end of pool block.  The most negative number which will  fit  in  a
   bufsize, defined in a way that the compiler will accept. */

#define ESent	((bufsize) (-(((1L << (sizeof(bufsize) * 8 - 2)) - 1) * 2) - 2))

/*  BGET  --  Allocate a buffer.  */
void * malloc(bufsize requested_size)
{
    bufsize size = requested_size;
    struct bfhead *b;
    void *buf;
    if (size < SizeQ) { /* Need at least room for the */
        size = SizeQ;   /*    queue links.  */
    }

    size = (size + (SizeQuant - 1)) & (~(SizeQuant - 1));
    size += sizeof(struct bhead);     /* Add overhead in allocated buffer to size required. */

    b = freelist.ql.flink;
    /* Scan the free list searching for the first buffer big enough to hold the requested size buffer. */

    while (b != &freelist)
    {        
/* Buffer  is big enough to satisfy  the request.  Allocate it
    to the caller.  We must decide whether the buffer is  large
    enough  to  split  into  the part given to the caller and a
    free buffer that remains on the free list, or  whether  the
    entire  buffer  should  be  removed	from the free list and
    given to the caller in its entirety.   We  only  split  the
    buffer if enough room remains for a header plus the minimum
    quantum of allocation. */
        if ((bufsize) b->bh.bsize >= size)
        {
            if ((b->bh.bsize - size) > (SizeQ + (sizeof(struct bhead))))
            {
                struct bhead *ba, *bn;
                ba = BH(((char *) b) + (b->bh.bsize - size));
                bn = BH(((char *) ba) + size);
                /* Subtract size from length of free block. */
                b->bh.bsize -= size;
                /* Link allocated buffer to the previous free buffer. */
                ba->prevfree = b->bh.bsize;
                /* Plug negative size into user buffer. */
                ba->bsize = -(bufsize) size;
                /* Mark buffer after this one not preceded by free block. */
                bn->prevfree = 0;
                totalloc += size;
                /* Increment number of bget() calls */
                numget++;

                buf = (void *) ((((char *) ba) + sizeof(struct bhead)));

                return buf;

            }
            else
            {
                struct bhead *ba;
                ba = BH(((char *) b) + b->bh.bsize);
                /* The buffer isn't big enough to split.  Give  the  whole
                shebang to the caller and remove it from the free list. */
                b->ql.blink->ql.flink = b->ql.flink;
                b->ql.flink->ql.blink = b->ql.blink;
                totalloc += b->bh.bsize;
                 /* Increment number of bget() calls */
                numget++;
                /* Negate size to mark buffer allocated. */
                b->bh.bsize = -(b->bh.bsize);
                /* Zero the back pointer in the next buffer in memory
                    to indicate that this buffer is allocated. */
                ba->prevfree = 0;
                /* Give user buffer starting at queue links. */
                buf =  (void *) &(b->ql);
                return buf;
            }
        }
        /* Link to next buffer */ 
        b = b->ql.flink;
    }
    /* no space found */
    return NULL;
}

/*  BGETZ  --  Allocate a buffer and clear its contents to zero.  We clear
	       the  entire  contents  of  the buffer to zero, not just the
	       region requested by the caller. */
#ifdef HAS_bgetz
void * bgetz(bufsize size)
{
    char *buf = (char *) bget(size);
    if (buf != NULL)
    {
        struct bhead *b;
        bufsize rsize;
        b = BH(buf - sizeof(struct bhead));
        rsize = -(b->bsize);
        if (rsize == 0)
        {
            struct bdhead *bd;
            bd = BDH(buf - sizeof(struct bdhead));
            rsize = bd->tsize - sizeof(struct bdhead);
        }
        else
        {
            rsize -= sizeof(struct bhead);
        }
        memset(buf, 0, (MemSize) rsize);
    }
    return ((void *) buf);
}
#endif
/*  BGETR  --  Reallocate a buffer.  This is a minimal implementation,
	       simply in terms of brel()  and  bget().	 It  could  be
	       enhanced to allow the buffer to grow into adjacent free
	       blocks and to avoid moving data unnecessarily.  */
void *realloc(void *buf, bufsize size)
{
    void *nbuf;
    bufsize osize; /* Old size of buffer */
    struct bhead *b;
    if ((nbuf = malloc(size)) == NULL) /* Acquire new buffer */
        return NULL;

    if (buf == NULL)
        return nbuf;

    b = BH(((char *) buf) - sizeof(struct bhead));
    osize = -b->bsize;
    osize -= sizeof(struct bhead);
    /* Copy the data */
    memcpy((char *) nbuf, (char *) buf,(MemSize) ((size < osize) ? size : osize));
    free(buf);
    return nbuf;
}

/*  BREL  --  Release a buffer.  */
void free(void *buf)
{
    struct bfhead *b, *bn;
    b = BFH(((char *) buf) - sizeof(struct bhead));
    /* Increment number of brel() calls */
    numrel++;

    /* Buffer size must be negative, indicating that the buffer is allocated. */
    if (b->bh.bsize >= 0) 
        bn = NULL;
        
    /*Back pointer in next buffer must be zero, indicating the same thing: */
    totalloc += b->bh.bsize;
    
    /* If the back link is nonzero, the previous buffer is free.  */
    if (b->bh.prevfree != 0)
    {
        /* The previous buffer is free.  Consolidate this buffer  with	it
            by  adding  the  length  of	this  buffer  to the previous free
            buffer.  Note that we subtract the size  in	the  buffer  being
            released,  since  it's  negative to indicate that the buffer is
            allocated. */
        register bufsize size = b->bh.bsize;
        
        /* Make the previous buffer the one we're working on. */
        b = BFH(((char *) b) - b->bh.prevfree);
        b->bh.bsize -= size;
    }
    else
    {
        /* The previous buffer isn't allocated.  Insert this buffer
            on the free list as an isolated free block. */
        b->ql.flink = &freelist;
        b->ql.blink = freelist.ql.blink;
        freelist.ql.blink = b;
        b->ql.blink->ql.flink = b;
        b->bh.bsize = -b->bh.bsize;
    }
    
    /* Now we look at the next buffer in memory, located by advancing from
       the  start  of  this  buffer  by its size, to see if that buffer is
       free.  If it is, we combine  this  buffer  with	the  next  one	in
       memory, dechaining the second buffer from the free list. */

    bn =  BFH(((char *) b) + b->bh.bsize);

    if (bn->bh.bsize > 0)
    {
        /* The buffer is free.	Remove it from the free list and add
            its size to that of our buffer. */
            
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

    /* The next buffer is allocated.  Set the backpointer in it  to  point
       to this buffer; the previous free buffer in memory. */
    bn->bh.prevfree = b->bh.bsize;
}

/*  BPOOL  --  Add a region of memory to the buffer pool.  */
void mem_addPool(void *buf,bufsize len)
{
    struct bfhead *b = BFH(buf);
    struct bhead *bn;
    len &= ~(SizeQuant - 1);

    /* Since the block is initially occupied by a single free  buffer,
       it  had	better	not  be  (much) larger than the largest buffer
       whose size we can store in bhead.bsize. */

    /* Clear  the  backpointer at  the start of the block to indicate that
       there  is  no  free  block  prior  to  this   one.    That   blocks
       recombination when the first block in memory is released. */

    b->bh.prevfree = 0;

    /* Chain the new block to the free list. */
    b->ql.flink = &freelist;
    b->ql.blink = freelist.ql.blink;
    freelist.ql.blink = b;
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
    bn = BH(((char *) b) + len);
    bn->prevfree = (bufsize) len;

    /* Definition of ESent assumes two's complement! */
    bn->bsize = ESent;
}

/*  BSTATS  --	Return buffer allocation free space statistics.  */
void mem_stat(bufsize *curalloc, bufsize *totfree, bufsize *maxfree,long * nget, long *nrel)
{
    struct bfhead *b = freelist.ql.flink;
    *nget = numget;
    *nrel = numrel;
    *curalloc = totalloc;
    *totfree = 0;
    *maxfree = -1;
    while (b != &freelist)
    {	
        *totfree += b->bh.bsize;
        if (b->bh.bsize > *maxfree)
            *maxfree = b->bh.bsize;
        /* Link to next buffer */
        b = b->ql.flink;
    }
}


/*  mem_stat  --	Dump a buffer pool.  The buffer headers are always listed.
		If DUMPALLOC is nonzero, the contents of allocated buffers
		are  dumped.   If  DUMPFREE  is  nonzero,  free blocks are
		dumped as well.  If FreeWipe  checking	is  enabled,  free
		blocks	which  have  been clobbered will always be dumped. */
void mem_dump(void *buf,int dumpalloc,int dumpfree)
{
    struct bfhead *b = BFH(buf);
    while (b->bh.bsize != ESent)
    {
        bufsize bs = b->bh.bsize;
        if (bs < 0)
        {
            bs = -bs;
            printk("Allocated buffer: size %6ld bytes.\n", (long) bs);
            if (dumpalloc)
                bufdump((void *) (((char *) b) + sizeof(struct bhead)));
        }
        else
        {
            char *lerr = "";
            if ((b->ql.blink->ql.flink != b) || (b->ql.flink->ql.blink != b))
                lerr = "  (Bad free list links)";
            printk("Free block:       size %6ld bytes.%s\n",(long) bs, lerr);
            if (dumpfree)
                bufdump((void *) (((char *) b) + sizeof(struct bhead)));
        }
        b = BFH(((char *) b) + bs);
    }
}

/*  BUFDUMP  --  Dump the data in a buffer.  This is called with the  user
		 data pointer, and backs up to the buffer header.  It will
		 dump either a free block or an allocated one.	*/
void bufdump(void *buf)
{
    struct bfhead *b;
    unsigned char *bdump;
    bufsize bdlen;
    
    b = BFH(((char *) buf) - sizeof(struct bhead));

    if (b->bh.bsize < 0)
    {
        bdump = (unsigned char *) buf;
        bdlen = (-b->bh.bsize) - sizeof(struct bhead);
    }
    else
    {
        bdump = (unsigned char *) (((char *) b) + sizeof(struct bfhead));
        bdlen = b->bh.bsize - sizeof(struct bfhead);
    }

    while (bdlen > 0)
    {
        int i, dupes = 0;
        bufsize l = bdlen;
        char bhex[50], bascii[20];
        if (l > 16)
            l = 16;
    
        for (i = 0; i < l; i++)
        {
            sprintf(bhex + i * 3, "%02X ", bdump[i]);
            bascii[i] = isprint(bdump[i]) ? bdump[i] : ' ';
        }
        
        bascii[i] = 0;
        printk("%-48s   %s\n", bhex, bascii);
        bdump += l;
        bdlen -= l;
    
        while ((bdlen > 16) && (memcmp((char *) (bdump - 16),(char *) bdump, 16) == 0))
        {
            dupes++;
            bdump += 16;
            bdlen -= 16;
        }
    
        if (dupes > 1)
            printk("     (%d lines [%d bytes] identical to above line skipped)\n",dupes, dupes * 16);
        else
            if(dupes == 1)
            {
                bdump -= 16;
                bdlen += 16;
            }
    }
}

