/*

    Interface definitions for bget.c, the memory management package.

*/

#ifndef _
#ifdef PROTOTYPES
#define  _(x)  x		      /* If compiler knows prototypes */
#else
#define  _(x)  ()                     /* It it doesn't */
#endif /* PROTOTYPES */
#endif

typedef long bufsize;

#define MemSize     int 	      /* Type for size arguments to memxxx()
					 functions such as memcmp(). */

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

#ifdef BECtl

/* Automatic expansion block management functions */

static int (*compfcn) _((bufsize sizereq, int sequence)) = NULL;
static void *(*acqfcn) _((bufsize size)) = NULL;
static void (*relfcn) _((void *buf)) = NULL;

static bufsize exp_incr = 0;	      /* Expansion block size */
static bufsize pool_len = 0;	      /* 0: no bpool calls have been made
					 -1: not all pool blocks are
					     the same size
					 >0: (common) block size for all
					     bpool calls made so far
				      */
#endif

/*  Minimum allocation quantum: */

#define QLSize	(sizeof(struct qlinks))
#define SizeQ	((SizeQuant > QLSize) ? SizeQuant : QLSize)

#define V   (void)		      /* To denote unwanted returned values */

/* End sentinel: value placed in bsize field of dummy block delimiting
   end of pool block.  The most negative number which will  fit  in  a
   bufsize, defined in a way that the compiler will accept. */

#define ESent	((bufsize) (-(((1L << (sizeof(bufsize) * 8 - 2)) - 1) * 2) - 2))
;
class BGET_T{

 public:
  
  struct bfhead freelist ;
  /*= {    
    {0, 0},
    {&freelist, &freelist}
    };*/

#ifdef BufStats
  bufsize totalloc ;	      /* Total space currently allocated */
  long numget , numrel ;   /* Number of bget() and brel() calls */
#ifdef BECtl
  long numpblk ;	      /* Number of pool blocks */
  long numpget , numprel ; /* Number of block gets and rels */
  long numdget , numdrel ; /* Number of direct gets and rels */
#endif /* BECtl */
#endif /* BufStats */

  char *bchain ;	      /* Our private buffer chain */
  char *bp ; 	      /* Our initial buffer pool */

  BGET_T(void ){
  	#if 0
    freelist = {     /* List of free buffers */
      {0, 0},
      {&freelist, &freelist}
    };
	#else
	freelist.bh.bsize = 0;
	freelist.bh.prevfree = 0;
	freelist.ql.blink = freelist.ql.flink = &freelist;
	#endif

    bchain = bp = NULL;

#ifdef BufStats
    totalloc = 0;
    numget = numrel = 0;
#ifdef BECtl
    numpblk = 0;
    numpget = numprel =0;
    numdget = numdrel =0;
#endif /* BECtl */
#endif /* BufStats */
    //    return NULL;
  }//BGET_T()

  void	bpool	    (void *buffer, bufsize len);
  //  void  *bget	    _((bufsize size));
  void  *bget	    (bufsize size);
  void  *bgetz	    (bufsize size);
  void  *bgetr	    (void *buffer, bufsize newsize);
  void	brel	    (void *buf);
  void	bectl	    (int (*compact)(bufsize sizereq, int sequence),
		       void *(*acquire)(bufsize size),
		       void (*release)(void *buf), bufsize pool_incr);
  void	bstats	    (bufsize *curalloc, bufsize *totfree, bufsize *maxfree,
		       long *nget, long *nrel);
  void	bstatse     (bufsize *pool_incr, long *npool, long *npget,
		       long *nprel, long *ndget, long *ndrel);
  void	bufdump     (void *buf);
  void	bpoold	    (void *pool, int dumpalloc, int dumpfree);
  int	bpoolv	    (void *pool);


  void * bexpand(bufsize size);
  int bcompact(bufsize bsize, int seq);
  void stats(char * when);
  void srand(unsigned int seed);
  int rand(void);
  void bshrink(void* buf);
  bufsize blimit(bufsize bs);
  int test_main(void);
};//class BGET_T
