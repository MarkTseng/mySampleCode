#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXV            0x7FFFFFFF
#define MINV            0x80000000
#define SAT(flt_num)    ((int)(flt_num >= (double)0x80000000?MAXV:flt_num))
#define Q31(flt_num)    (SAT((double)0x80000000 * (flt_num) +0.5))

#define Q_XY            29      // Depth_Value and LR_Value [-0x20000000,0x20000000]
#define Q_SIN           6       // sin table size (2^Q_SIN+1)
#define TABLE_SIZE      8      // (2^Q_SIN+1)

int sin_table[TABLE_SIZE] = {
	Q31(0.000000000000),
	Q31(0.024541228523),
	Q31(0.049067674327),
	Q31(0.073564563600),
	Q31(0.098017140330),
	Q31(0.122410675199),
	Q31(0.146730474455),
	Q31(0.170961888760),
};

int main()
{
	double r,t1,t2;
	t1 = 200.00;
	t2 = 433.2;
	
	r = t1/t2;
	printf("r = %f\n",r);
	r = t1-t2;
	printf("r = %f\n",r);
	return 0;
}
