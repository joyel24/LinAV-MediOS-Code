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
#define MAGIC_MALLOC   0x4d454f53

#define BH(p)   ((struct bhead *) (p))

struct bhead *  pool_start;
struct bhead *  first_free;

unsigned int totalloc = 0;

/*  malloc  --  Allocate a buffer.  */

void * malloc(unsigned int  size)
{
    return internalMalloc(size,THREAD_NO_FORCE);
}

void * kmalloc(unsigned int  size)
{
    return internalMalloc(size,THREAD_FORCE);
}

void * internalMalloc(unsigned int  requested_size,int isKernel)
{
    unsigned int  size = requested_size;
    struct bhead *b;
    void *buf;

    //printk("Doing %s malloc: s=%x, first_free=%x (%x)\n",isKernel?"kernel":"std",requested_size,first_free,&first_free);

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
                b_alloc->magic_val=MAGIC_MALLOC;
                b_alloc->blist.nxt=b->blist.nxt;
                if(b_alloc->blist.nxt)
                    b_alloc->blist.nxt->blist.prev=b_alloc;
                b->blist.nxt=b_alloc;
                b_alloc->blist.prev=b;

                /* adding this new malloc to the list of current thread */
                thread_listAdd(THREAD_PTR_2_LIST(b_alloc),MEM_RESSOURCE,isKernel);

                totalloc += size;

                buf = (void *) ((((char *) b_alloc) + sizeof(struct bhead)));
                //printk("malloc: %x, firstFree=%x\n",buf,first_free);
                //mem_printShortStat();
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
                b->magic_val=MAGIC_MALLOC;
                thread_listAdd(THREAD_PTR_2_LIST(b),MEM_RESSOURCE,isKernel);

                buf =  (void *) ((((char *) b) + sizeof(struct bhead)));
                //printk("malloc: %x, firstFree=%x\n",buf,first_free);
                //mem_printShortStat();
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

void * realloc(void *buf,unsigned int size)
{
    return internalRealloc(buf,size,0);
}

void * krealloc(void *buf,unsigned int size)
{
    return internalRealloc(buf,size,1);
}

void * internalRealloc(void *buf,unsigned int size,int isKernel)
{
    void *nbuf;
    unsigned int osize; /* Old size of buffer */
    struct bhead *b;
    if ((nbuf = internalMalloc(size,isKernel)) == NULL) /* Acquire new buffer */
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

#define TEST_NULL(BUFF) {if(BUFF==NULL) {printk("[FREE] NULL buffer\n"); return;}}
#define TEST_MAGIC(PTR) {if(PTR->magic_val!=MAGIC_MALLOC) {printk("[FREE] not a malloc buffer\n"); return;}}
/*  free  --  Release a buffer.  */
void free(void *buf)
{
    TEST_NULL(buf)
    struct bhead * b = BH(((char *) buf) - sizeof(struct bhead));
    TEST_MAGIC(b) ;
    if(thread_listRm(THREAD_PTR_2_LIST(b),MEM_RESSOURCE,THREAD_NO_FORCE)==MED_OK)
    /* doing real free only if allowed to */
        internalFree(b);
}

void kfree(void * buf)
{
    TEST_NULL(buf)
    struct bhead * b = BH(((char *) buf) - sizeof(struct bhead));
    TEST_MAGIC(b) ;
    thread_listRm(THREAD_PTR_2_LIST(b),MEM_RESSOURCE,THREAD_FORCE);
    /* forcing and we don't care of the result */
    internalFree(b);
}

void internalFree(struct bhead * b)
{
    struct bhead * b_prev;
    struct bhead * b_nxt;
    /* Buffer size must be negative, indicating that the buffer is allocated. */
    if (b->size >= 0)
        return;

    b->size = - b->size;

    //printk("Free: %x/%x\n",b,(unsigned long)((((char *) b) + sizeof(struct bhead))));

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
            MERGE_BLOCK(b_prev,b);
            MERGE_BLOCK(b_prev,b_nxt);
            REMOVE_FREE(b_prev);
            REMOVE_FREE(b_nxt);
            INSERT_FREE(b_prev);

        }
        else
        {
            /* only merge 2 blocks */
            MERGE_BLOCK(b_prev,b);
            REMOVE_FREE(b_prev);
            INSERT_FREE(b_prev);
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
            MERGE_BLOCK(b,b_nxt);
            REMOVE_FREE(b_nxt);
            INSERT_FREE(b);
        }
        else
        {
            //printk("No merge\n");
            INSERT_FREE(b);
        }
    }
}

/*  BPOOL  --  Add a region of memory to the buffer pool.  */

void mem_init(void * start,unsigned int len)
{
    mem_addPool(start,len);
    THREAD_INIT_RES(struct bhead,internalFree,MEM_RESSOURCE,mem_printItem);
}

void mem_addPool(void *buf,unsigned int len)
{
    struct bhead *b = BH(buf);

    first_free = b;
    pool_start = b;

    b->size=len;
    b->blist.nxt=b->blist.prev=NULL;
    b->free_list.nxt=b->free_list.prev=NULL;
    thread_listIni(THREAD_PTR_2_LIST(b));
}

void printExtra(void)
{
    struct bhead *b = pool_start;
    while (b != NULL)
    {
        if(b->size<0 && b->__thread_list.pid==-1)
        {
            mem_printItem(b);
        }
        b = b->blist.nxt;
    }
}

void mem_printShortStat(void)
{
    unsigned int curalloc,totfree, maxfree;
    mem_stat(&curalloc,&totfree,&maxfree);
    printk("Free: %x | alloc %x | biggest free block: %x\n",
        totfree,curalloc,maxfree);
}

void mem_printStat(void)
{
    mem_printShortStat();
    mem_freeList();
    printk("Thread buffer:\n");
    thread_listPrintAll(MEM_RESSOURCE);
    printk("Extra buffer(pid=-1)\n");
    printExtra();
}


void mem_stat(unsigned int *curalloc,
            unsigned int *totfree, unsigned int *maxfree)
{
    struct bhead *b = pool_start;
    int nb=0;
    *totfree = 0;
    *curalloc = 0;
    *maxfree = 0;
    while (b != NULL)
    {
        nb++;
        if(b->size>=0)
        {
            *totfree+= b->size;
             if (b->size > *maxfree)
                *maxfree = b->size;
        }
        else
        {
            *curalloc-= b->size;
        }

        b = b->blist.nxt;
    }
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

void mem_printItem(struct bhead * ptr)
{
    printk("Buffer: %x size = %x\n",(unsigned long)((((char *) ptr) + sizeof(struct bhead))),-ptr->size);
}
