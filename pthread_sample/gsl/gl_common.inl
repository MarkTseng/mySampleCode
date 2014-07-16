#ifndef __GSL_INLINE_H__
#define __GSL_INLINE_H__

#include "gsl/gl_config.h"
#include "gsl/gl_types.h"

static void gsl_copy_name(char *dest, char *src)
{
	if (src && dest) {
		memset(dest, 0, GSL_NAME_LEN + 1);
		strncpy(dest, src, GSL_NAME_LEN);
	}
}

#if 0
GSL_INLINE void to_ecos_tick(cyg_tick_count_t *out, cyg_uint32 in)
{
	*out = (in + GSL_OS_TICK - 1) / GSL_OS_TICK;
}

GSL_INLINE void to_gsl_tick(cyg_uint32 *out, cyg_uint32 in)
{
	*out = in * GSL_OS_TICK;
}
#endif

#ifdef GL_OBJECT_LIST
extern void gsl_insert_list(void *head, void *elem);
extern int gsl_remove_list(void *phead, void *pelem);
extern void gsl_insert_dlist(void *inserted, void *insert);
extern void gsl_remove_dlist(void *removed);
#endif

#endif

