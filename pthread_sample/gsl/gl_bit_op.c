#include "gsl.h"

GL_UINT32 GL_ReadBits (GL_UINT32 dAddr, GL_UINT32 dBitMask, GL_UINT8 bLsb)
{
	register GL_UINT32 *addr = (GL_UINT32*) dAddr;
	return ((*addr) & dBitMask) >> bLsb;
}

void GL_WriteBits(GL_UINT32 dAddr, GL_UINT32 dBitMask, GL_UINT8 bLsb, GL_UINT32 dBitData)
{
	register GL_UINT32 *addr = (GL_UINT32*) dAddr;
	*addr |= (dBitData << bLsb) & dBitMask;
}
