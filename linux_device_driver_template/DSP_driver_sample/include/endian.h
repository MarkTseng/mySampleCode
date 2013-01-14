#ifndef	__ENDIAN_H
#define	__ENDIAN_H

#define	BIT_L2M(x)	( (((x)&0x01)<<7) | (((x)&0x02)<<5) | \
			  (((x)&0x04)<<3) | (((x)&0x08)<<1) | \
			  (((x)&0x10)>>1) | (((x)&0x20)>>3) | \
			  (((x)&0x40)>>5) | (((x)&0x80)>>7))

#define	ADR_L2M(x)	(((x)&(~0x07)) | (7-((x)&0x07)))

//
// SWAP
//
//	AABB	 	BBAA
//	AABBCCDD	DDCCBBAA
//
#define	SWAP16(x)	(((x)&0x0ff)<<8 | ((x)>>8))
#define	SWAP32(x)	( (((UINT32)x           ) >> 24)	\
			| (((UINT32)x&0x00ff0000) >>  8)	\
			| (((UINT32)x<<8) & 0x00ff0000)		\
			| (((UINT32)x		) << 24))		


//
//
#if 0
extern  void	GL_wbe32(void *, UINT32);
extern  void	GL_wbe16(void *, UINT32);
extern  void	GL_wle32(void *, UINT32);
extern  void	GL_wle16(void *, UINT32);

extern  UINT32	GL_lbe32(void *);
extern  UINT32	GL_lbe16(void *);
extern  UINT32	GL_lle32(void *);
extern  UINT32	GL_lle16(void *);
#endif

#include "gsl/gl_endian.h"

#if	defined(__MIPSEL__)
	#define	SYSTEM_ENDIAN_LITTLE
#elif	defined(__MIPSEB__)
	#define	SYSTEM_ENDIAN_BIG
#else
	#error	"please define endian"
#endif

#define	wbe32(p,d)	GL_wbe32((BYTE *)p,d)
#define	wbe16(p,d)	GL_wbe16((BYTE *)p,d)
#define	lbe32(p)		GL_lbe32((BYTE *)p)
#define	lbe16(p)		GL_lbe16((BYTE *)p)
#define	wle32(p,d)	GL_wle32((BYTE *)p,d)
#define	wle16(p,d)	GL_wle16((BYTE *)p,d)
#define	lle32(p)		GL_lle32((BYTE *)p)
#define	lle16(p)		GL_lle16((BYTE *)p)
#define	lbe8(p)		(*(UINT8 *)p)
#define	lle8(p)		(*(UINT8 *)p)

#if 0
#if	defined(SYSTEM_ENDIAN_LITTLE)
//
// LITTLE endian
//
#define	wbe32(p,d)	GL_wbe32(p,d)
#define	wbe16(p,d)	GL_wbe16(p,d)
#define	lbe32(p)	GL_lbe32(p)
#define	lbe16(p)	GL_lbe16(p)
#define	lbe8(p)		(*(UINT8 *)p)

#define	wle32(p,d)	(*(UINT32 *)p = d)
#define	wle16(p,d)	(*(UINT16 *)p = d)
#define	lle32(p)	(*(UINT32 *)p)
#define	lle16(p)	(*(UINT16 *)p)
#define	lle8(p)		(*(UINT8 *)p)
#elif	defined(SYSTEM_ENDIAN_BIG)
//
// BIG endian
//
#define	wbe32(p,d)	(*(UINT32 *)p = d)
#define	wbe16(p,d)	(*(UINT16 *)p = d)
#define	lbe32(p)	(*(UINT32 *)p)
#define	lbe16(p)	(*(UINT16 *)p)
#define	lbe8(p)		(*(UINT8 *)p)

#define	wle32(p,d)	GL_wle32(p,d)
#define	wle16(p,d)	GL_wle16(p,d)
#define	lle32(p)	GL_lle32(p)
#define	lle16(p)	GL_lle16(p)
#define	lle8(p)		(*(UINT8 *)p)
#endif
#endif

#endif/*__ENDIAN_H*/
