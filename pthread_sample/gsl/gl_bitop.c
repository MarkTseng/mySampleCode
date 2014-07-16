#include "gsl.h"
//
// getbits utility for gunzip
// *note* this getbits utility is:
// 1. byte-oriented
// 2. LSB first
//

// t_getbit
typedef struct {
    GL_UINT8 *p;
    int   r;
} t_getbit;

// dumpbits()
#define	dumpbits(p,n)	(((p)->r)+=(n))

//
// FUNCTION
// revn
// DESCRIPTION
// reverse bit-wisely:  w[31:0] -> w[0:31]
//          w[7:0] -> w[0:7]
//
unsigned revn(unsigned u, int n)
{
	unsigned v = 0;

	for (; n > 0; n--) {
		v = (v << 1) | (u & 0x01);
		u = (u >> 1);
	}
	return v;
}

//
// FUNCTION
// showbit
// DESCRIPTION
// show following 24-bits
// *NOTE* not-masked
//
unsigned showbit(t_getbit * p)
{
	unsigned v;
	unsigned r;
	GL_UINT8 *q;

	r = p->r;
	q = p->p + (r / 8);
	v = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
	return v >> (r % 8);
}

// maskbit()
static	inline
unsigned
maskbit(unsigned b, unsigned n)
{
    GL_UINT8 mask = (n==32) ? 0xffffffffu : (1u<<n)-1;
    return b & mask;
}

// getbits()
static inline unsigned
getbits(t_getbit *p, int n)
{
    unsigned v = maskbit(showbit(p),n);
    dumpbits(p,n);
    return v;
}

// getbits_bytealign()
static	inline
void
getbits_bytealign(t_getbit *p)
{
    p->r=(p->r+7) & ~7;
}

//
// FUNCTION
// maskbit
// DESCRIPTION
// extract 0~31 LSB. (32-bit mask is not supported)
//
#if 0
unsigned maskbit(unsigned b, int n)
{
	return b & ((1 << n) - 1);
}
#endif

//
// gunzip inflate-fixed lookup tables
//

// i = idx-257  (idx: 257~285 escape-code)
// length: 3 + tbl_lel3[] + getbits(gb,tbl_lee[])

// tbl_le3[]: length offset-3   (-3 to compress it into 8-bit table)
const GL_UINT8 tbl_lel3[] = {
	0, 1, 2, 3, 4, 5, 6, 7,
	8, 10, 12, 14,
	16, 20, 24, 28,
	32, 40, 48, 56,
	64, 80, 96, 112,
	128, 160, 192, 224,
	255
};

// tbl_lee[]: length extra bits
const GL_UINT8 tbl_lee[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1,
	2, 2, 2, 2,
	3, 3, 3, 3,
	4, 4, 4, 4,
	5, 5, 5, 5,
	0
};

// f = getbits(gb, 5)
// distance = tbl_dd[f] + getbits(gb, tbl_de[f])

// tbl_dd[]: distance offset
const GL_UINT16 tbl_dd[] = {
	1, 2, 3, 4, 5, 7, 9, 13,
	17, 25, 33, 49, 65, 97, 129, 193,
	257, 385, 513, 769, 1025, 1537, 2049, 3073,
	4097, 6145, 8193, 12289, 16385, 24577
};

// tbl_de[]: distance extra-bits
const GL_UINT8 tbl_de[] = {
	0, 0, 0, 0, 1, 1, 2, 2,
	3, 3, 4, 4, 5, 5, 6, 6,
	7, 7, 8, 8, 9, 9, 10, 10,
	11, 11, 12, 12, 13, 13
};

static inline int inflate_fixed(t_getbit * gb, GL_UINT8 * op, int wlimit)
{
	int w;
	w = 0;
	while (1) {
		unsigned int u, v;

		v = revn(showbit(gb), 9);
		if (v < 0x60) {
			u = 256 + (v >> 2);
			dumpbits(gb, 7);
		} else if (v < 0x0190) {
			v >>= 1;
			if (v < 0xC0) {
				u = v - 0x30;
			} else {
				u = v - 0xC0 + 280;
			}
			dumpbits(gb, 8);
		} else {
			u = v - 0x0190 + 144;
			dumpbits(gb, 9);
		}

		if (u == 256) {
			break;				// eob
		} else if (u < 256) {
			op[w++] = u;		// literal
		} else if (u <= 285) {
			int l, d, e;		// len/dist/extra

			u -= 257;
			e = tbl_lee[u];
			l = tbl_lel3[u] + 3 + getbits(gb, e);	// extra
			u = revn(getbits(gb, 5), 5);
			e = tbl_de[u];
			d = tbl_dd[u] + getbits(gb, e);	// extra
			//diag_printf("\t\t[%d %d]\n", d, l);
			do {
				op[w] = op[w - d];
				w++;
				if (w >= wlimit) {
					return -1;	// limit it
				}
			} while (--l > 0);
		} else {
			return -3;
		}
		if (w >= wlimit) {
			return -1;			// limit it
		}
	}
	return w;
}

//#define CHECK_CRC
//#define CHECK_LEN

#ifdef CHECK_CRC
UINT32 calcrc32(const UINT8 * p, int l)
{
#define POLY	0xEDB88320
	UINT32 crc32 = 0xFFFFFFFFul;

	while (l-- > 0) {
		int i;
		UINT8 c;

		c = *p++;
		for (i = 8; i; i--) {
			unsigned crcb = (crc32 ^ c) & 0x01;
			crc32 = (crcb << 31) | ((crcb) ? (crc32 >> 1) ^ POLY : crc32 >> 1);
			c = c >> 1;
		}
	}
	return crc32 ^ 0xFFFFFFFFul;
}
#endif

//
// FUNCTION
// gunzip
// DESCRIPTION
// deflate a fixed-inflated source (initialized with getbit)
//
int gunzip(t_getbit *gb, GL_UINT8 *op)
{
	int w;
	int wlimit = 0x7FFFFFFF;
	GL_UINT32 u;
	GL_UINT32 crc32;
	GL_UINT32 len32;

	w = 0;
	do {
		u = getbits(gb, 3);
		if ((u & 0x06) == 0x02) {
			int v = inflate_fixed(gb, op + w, wlimit);
			if (v < 0) {
				return v;
			}
			w += v;
			wlimit -= v;
		} else {
			return -1;			// multi-part
		}
	} while (!(u & 0x01));

	// only calculate crc at end of input
	getbits_bytealign(gb);		// following fields are byte-aligned
	crc32 = getbits(gb, 32);	// fetch crc
	len32 = getbits(gb, 32);	// fetch length (output)

#ifdef CHECK_CRC
	u = calcrc32(op, w);		// calculate crc (of origin data)
#ifdef STANDALONE
	diag_printf("crc32 %08x %08x len %08x %08x\n", crc32, u, len32, w);
#endif
	if (crc32 != u) {
		return -2;				// crc-error
	}
#endif

#ifdef  CHECK_LEN
	if (len32 != w) {
		return -3;				// length-error
	}
#endif
	return w;
}

//
// STANDALONE test program
//
#ifdef	STANDALONE

#include "stdio.h"

#define	ISIZE 60000
#define	OSIZE 300000
GL_UINT8 bufi[ISIZE];
GL_UINT8 bufo[OSIZE];

char *prgname;

int main(int argc, char **argv)
{
	FILE *ifile, *ofile;
	t_getbit gb;
	int l;

	prgname = argv[0];
	argc--;
	argv++;
	if (argc < 2) {
		fprintf(stderr, "iofile\n");
		exit(-1);
	}
	ifile = fopen(argv[0], "rb");
	ofile = fopen(argv[1], "wb");
	if (ifile == NULL) {
		fprintf(stderr, "open error\n");
		exit(-1);
	}
	fread(bufi, 1, ISIZE, ifile);

	getbits_init(&gb, bufi);
	l = gunzip(&gb, bufo);
	fwrite(bufo, 1, l, ofile);
	diag_printf("len %d ($%x)\n", l, l);

	exit(0);
}

#endif

