#include "gsl.h"

#ifdef CYGPKG_PROFILE_GPROF
#include <cyg/profile/profile.h>

GL_Status_t GL_ProfileOn(void *_start, void *_end, int _bucket_size, int resolution)
{
	static int flag = 0;

	if (flag != 0) {
		diag_printf("Profiling had already been on!\n");
		return GL_FAILURE;
	}
	flag = 1;
	profile_on(_start, _end, _bucket_size, resolution);
	return GL_SUCCESS;
}

void GL_ProfilePause(void)
{
	profile_pause();
}

void GL_ProfileResume(int opt)
{
	opt = opt & 0x01;
	profile_resume(opt);
}
#endif

