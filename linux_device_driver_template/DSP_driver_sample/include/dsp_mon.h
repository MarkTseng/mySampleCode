#ifndef __DSP_MON_

#define __DSP_MON_

void dumpLongDataLE(UINT32 base, UINT32 len);
const UINT8 *mon_skipspace(const UINT8 * p);
const UINT8 *mon_readhex(const UINT8 * p, int *x);
void dsp_monCmd(const char* pbuf);

#endif
