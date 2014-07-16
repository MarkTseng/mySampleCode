#include <stdarg.h>
#include <gsl.h>
int __wrap_diag_printf(const char *fmt, ...)
{
	va_list ap;
	int ret;

	printf("[%08X] ", GL_GetTimestamp());
	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);

	return ret;
}

