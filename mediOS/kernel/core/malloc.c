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

/* Queue links */
struct qlinks {
    struct bhead *nxt;        /* Forward link */
    struct bhead *prev;       /* Backward link */
};

/* Header in allocated and free buffers */
struct bhead {
    int size;
    int user_space;
    struct qlinks blist;
    struct qlinks free_list;
};

#define BH(p)   ((struct bhead *) (p))

struct bhead *  pool_start;
struct bhead *  first_free;

unsigned int totalloc = 0;

void * malloc(unsigned int  requested_size)
{
   return do_malloc(requested_size,0);
}

/*  malloc  --  Allocate a buffer.  */
void * do_malloc(unsigned int  requested_size,int user_flag)
{
    unsigned int  size = requested_size;
    struct bhead *b;
    void *buf;

    size = (size + (SizeQuant - 1)) & (~(SizeQuant - 1));
    size += sizeof(struct bhead);     /* Add overhead in allocated buffer to size required. */

    b = first_free;
    /* Scan the free list searching for the first buffer big enough to hold the requested size buffer. */

    while (b != NULL)
    {
        if (b->size >= size)
        {
            if ((b->size - size) > (SizeQuant + (sizeof(struct bhead))))
            {
                /* buffer big enough to split */
                struct bhead *b_alloc;
                b_alloc = BH(((char *) b) + (b->size - size));
                b->size -= size;
                b_alloc->size=-size;
                b_alloc->blist.nxt=b->blist.nxt;
                if(b_alloc->blist.nxt)
                    b_alloc->blist.nxt->blist.prev=b_alloc;
                b->blist.nxt=b_alloc;
                b_alloc->blist.prev=b;
                b_alloc->user_space = user_flag;

                totalloc += size;

                buf = (void *) ((((char *) b_alloc) + sizeof(struct bhead)));
                return buf;
            }
            else
            {

                if(b->free_list.prev==NULL)
                {
                    first_free=b->free_list.nxt; /* trying to get first free */
                    first_free->free_list.prev=NULL;
                }
                else
                {
                    b->free_list.prev->free_list.nxt=b->free_list.nxt; /* remove from free list */
                    if(b->free_list.nxt)
                        b->free_list.nxt->free_list.prev = b->free_list.prev;
                }
                /*nothing to be done for std linkage*/
                totalloc += b->size;
                b->size = -b->size;
                b->user_space = user_flag;

                buf =  (void *) ((((char *) b) + sizeof(struct bhead)));
                return buf;
            }
        }
        /* Link to next buffer */
        b = b->free_list.nxt;
    }
    /* no space found */
    return NULL;
}

/*  realloc  --  Reallocate a buffer.  This is a minimal implementation,
           simply in terms of brel()  and  bget().   It  could  be
           enhanced to allow the buffer to grow into adjacent free
           blocks and to avoid moving data unnecessarily.  */

void * user_realloc(void *buf,unsigned int requested_size)
{
   return do_realloc(buf,requested_size,1);
}

void * realloc(void *buf,unsigned int requested_size)
{
   return do_realloc(buf,requested_size,0);
}

void * do_realloc(void *buf,unsigned int size,int user_flag)
{
    void *nbuf;
    unsigned int osize; /* Old size of buffer */
    struct bhead *b;
    if ((nbuf = do_malloc(size,user_flag)) == NULL) /* Acquire new buffer */
        return NULL;

    if (buf == NULL)
        return nbuf;

    b = BH(((char *) buf) - sizeof(struct bhead));
    osize = -b->size;
    osize -= sizeof(struct bhead);
    /* Copy the data */
    memcpy((char *) nbuf, (char *) buf,(MemSize) ((size < osize) ? size : osize));
    free(buf);
    return nbuf;
}

#define MERGE_BLOCK(A,B) {               \
    A->blist.nxt = B->blist.nxt;         \
    if(A->blist.nxt)                     \
        A->blist.nxt->blist.prev = A ;   \
    A->size += B->size;                  \
}

/* this macro is not checking if we insert on top as it should always
insert AFTER START block */

#define INSERT_FREE_W_START(A,START) {    \
    struct bhead * __ptr=START;            \
    while(__ptr->free_list.nxt && __ptr->size < A->size) \
        __ptr=__ptr->free_list.nxt;        \
    if(__ptr->size >= A->size)             \
    {                                      \
        A->free_list.nxt=__ptr;            \
        A->free_list.prev=__ptr->free_list.prev; \
        __ptr->free_list.prev=A;           \
        A->free_list.prev->free_list.nxt=A;\
    }                                      \
    else                                   \
    {                                      \
        __ptr->free_list.nxt = A;          \
        A->free_list.prev = __ptr;         \
        A->free_list.nxt = NULL;           \
    }                 \
}

#define INSERT_FREE(A) {               \
    if(!first_free)                    \
    {                                  \
        first_free = A;                \
        A->free_list.prev=NULL;        \
        A->free_list.nxt=NULL;         \
    }                                  \
    else                               \
    {                                  \
        if(A->size <= first_free->size) \
        {                                \
            A->free_list.nxt = first_free;\
            first_free->free_list.prev=A; \
            A->free_list.prev=NULL;       \
            first_free=A;                 \
        }                                \
        else                            \
        {                              \
            INSERT_FREE_W_START(A,first_free); \
        }                              \
    }                                  \
}

#define MERGE_FREE(A,B) {    \
    A->free_list.prev = B->free_list.prev; \
    if(A->free_list.prev)                  \
        A->free_list.prev->free_list.nxt = A; \
    else                                   \
        first_free=A;                      \
    A->free_list.nxt = B->free_list.nxt; \
    if(A->free_list.nxt)                  \
        A->free_list.nxt->free_list.prev = A; \
}

#define REMOVE_FREE(A) {       \
    if(A==first_free)           \
    {                           \
        first_free=A->free_list.nxt; \
        if(first_free)                 \
            first_free->free_list.prev=NULL;     \
    }                          \
    else                       \
    {                           \
        A->free_list.prev->free_list.nxt = A->free_list.nxt; \
        if(A->free_list.nxt)        \
            A->free_list.nxt->free_list.prev = A->free_list.prev ; \
    }                          \
}

/*  free  --  Release a buffer.  */
void free(void *buf)
{
    struct bhead * b = BH(((char *) buf) - sizeof(struct bhead));
    struct bhead * b_prev;
    struct bhead * b_nxt;
    /* Buffer size must be negative, indicating that the buffer is allocated. */
    if (b->size >= 0)
        return;

    b->size = - b->size;

    totalloc -= b->size;

    /* If the back link is nonzero, the previous buffer is free.  */
    if (b->blist.prev && b->blist.prev->size>=0)
    {
        /* The previous buffer is free. Consolidate this buffer  with it*/
        b_prev=b->blist.prev;        
        b_nxt=b->blist.nxt;
        
        if(b_nxt && b_nxt->size>=0)
        {
            /* merge the 3 blocks */
            //printk("merge 3\n");
            MERGE_BLOCK(b_prev,b);
            MERGE_BLOCK(b_prev,b_nxt);
#if 0            
            /* link to next free */
            if(b_nxt->free_list.nxt && b_nxt->free_list.nxt->size > b_prev->size)
            {
                REMOVE_FREE(b_prev);
                REMOVE_FREE(b_nxt);
                b_nxt=b_nxt->free_list.nxt;
                INSERT_FREE_W_START(b_prev,b_nxt);
            }
            else
                REMOVE_FREE(b_nxt);
#else
            REMOVE_FREE(b_prev);
            REMOVE_FREE(b_nxt);
            INSERT_FREE(b_prev);
#endif     
                    
        }
        else
        {
            /* only merge 2 blocks */
            //printk("merge 2: prev - cur\n");
            MERGE_BLOCK(b_prev,b);
#if 0
            if(b_nxt && b_nxt->size>b_prev->size)
            {
                REMOVE_FREE(b_prev);
                INSERT_FREE_W_START(b_prev,b_nxt);
            }
#else
            REMOVE_FREE(b_prev);
            INSERT_FREE(b_prev);
#endif
        }
    }
    else
    {
        /* The previous buffer is allocated.  Insert this buffer
            on the free list as an isolated free block. */
        
        b_nxt=b->blist.nxt;
        
        /*is nxt block empty */
        if(b_nxt && b_nxt->size>=0)
        {
            /* merge the 2 blocks */
            //printk("merge 2: cur - nxt\n");
            MERGE_BLOCK(b,b_nxt);
#if 0
            if(b_nxt->free_list.nxt && b_nxt->free_list.nxt->size < b->size)
            {
                REMOVE_FREE(b_nxt);
                b_nxt=b_nxt->free_list.nxt;
                INSERT_FREE_W_START(b,b_nxt);
            }
            else
                MERGE_FREE(b,b_nxt);
#else
            REMOVE_FREE(b_nxt);
            INSERT_FREE(b);
#endif

        }
        else
        {
            //printk("No merge\n");
            INSERT_FREE(b);
        }
    }
}

void free_user(void)
{
    struct bhead *b = pool_start;
    unsigned int tot_free=0;
    int nb=0;
    while(b!=NULL)
    {
        if(b->size<0 && b->user_space)
        {
            printk("%d: Freeing @%x, size=%x\n",nb,b,-b->size);
            nb++;
            tot_free-=b->size;
            free((void *) ((((char *) b) + sizeof(struct bhead))));
        }
        b=b->blist.nxt;
    }
    printk("Have free %d user buffer tot size =%x\n",nb,tot_free);
}

/*  BPOOL  --  Add a region of memory to the buffer pool.  */
void mem_addPool(void *buf,unsigned int len)
{
    struct bhead *b = BH(buf);

    first_free = b;
    pool_start = b;

    b->size=len;
    b->user_space=0;
    b->blist.nxt=b->blist.prev=NULL;
    b->free_list.nxt=b->free_list.prev=NULL;
}

void mem_printStat(void)
{
    unsigned int curalloc_user,curalloc_kernel,totfree, maxfree;
    mem_stat(&curalloc_user,&curalloc_kernel,&totfree,&maxfree);
    printk("Free: %x | alloc USER: %x, kernel: %x | biggest free block: %x\n",
        totfree,curalloc_user,curalloc_kernel,maxfree);
}


void mem_stat(unsigned int *curalloc_user, unsigned int *curalloc_kernel,
            unsigned int *totfree, unsigned int *maxfree)
{
    struct bhead *b = pool_start;
    int nb=0;
    *totfree = 0;
    *curalloc_user = 0;
    *curalloc_kernel = 0;
    *maxfree = -1;
    while (b != NULL)
    {
        printk("%d: buffer @ %x %s/%s, size = %x\n",nb,b,b->size>=0?"free":"alloc",
            b->user_space?"user":"kernel",b->size>=0?b->size:-b->size);
        nb++;
        if(b->size>=0)
        {
            *totfree+= b->size;
             if (b->size > *maxfree)
                *maxfree = b->size;
        }
        else
        {
            if(b->user_space)
                *curalloc_user-= b->size;
            else
                *curalloc_kernel-= b->size;
        }

        b = b->blist.nxt;
    }
    printk("Buff start: %x first_free: %x nb alloc: %x tot=%x free=%x alloc user=%x kernel=%x\n",
        pool_start,first_free,nb,totalloc,*totfree,*curalloc_user,*curalloc_kernel);
        
    mem_freeList();
}

void mem_freeList(void)
{
    struct bhead *b = first_free;
    int nb=0;
    printk("Free list:\n");
    while (b != NULL)
    {
        printk("%d: buffer @ %x (%s) size = %x\n",nb++,b,b->size>=0?"free":"alloc",b->size>=0?b->size:-b->size);
        b = b->free_list.nxt;
    }
}
