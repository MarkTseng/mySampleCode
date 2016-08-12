// Consider a simple example:
#define PORTBASE 0x40000000
unsigned int volatile * const port = (unsigned int *) PORTBASE;
// The variable port is a constant pointer to a volatile unsigned integer, so we can access the memory-mapped register using:
*port = value; /* write to port */
value = *port; /* read from port */

#define PORTBASE 0x40000000 /* Counter/Timer Base */
#define PortLoad ((volatile unsigned int *) PORTBASE) /* 32 bits */
#define PortValue ((volatile unsigned short *)(PORTBASE + 0x04)) /* 16 bits */
#define PortClear ((volatile unsigned char *)(PORTBASE + 0x08)) /* 8 bits */ void init_regs(void)
{
unsigned int int_val;
unsigned short short_val;
unsigned char char_val; *PortLoad = (unsigned int) 0xF00FF00F;
int_val = *PortLoad;
*PortValue = (unsigned short) 0x0000;
short_val = *PortValue;
*PortClear = (unsigned char) 0x1F;
char_val = *PortClear;
}

