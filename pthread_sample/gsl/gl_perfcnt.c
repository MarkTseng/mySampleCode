#include "gsl.h"

GL_UINT32 GL_PerfCntSelect0(int event, int mode)
{
	return mode;
}

GL_UINT32 GL_PerfCntSelect1(int event, int mode)
{
	return mode;
}

GL_UINT32 GL_PerfCntRead0(void)
{
	return 0;
}

void GL_PerfCntWrite0(GL_UINT32 value)
{
}

GL_UINT32 GL_PerfCntRead1(void)
{
	return 0;
}

void GL_PerfCntWrite1(GL_UINT32 value)
{

}

