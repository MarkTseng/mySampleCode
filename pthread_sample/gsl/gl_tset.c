#include <gsl.h>
#include <platform/regmap.h>

int GL_TsetLock(TsetList n, unsigned int poll_interval)
{
	while (0 == HWREG_R(tset[n])) {
		GL_TaskSleep(poll_interval);
	}
	return 0;
}

int GL_TsetTryLock(TsetList n)
{
	/* 0: success; 1: failed.  */
	return (0 == HWREG_R(tset[n]));
}

void GL_TsetUnLock(TsetList n)
{
	HWREG_W(tset[n], 0);
}

