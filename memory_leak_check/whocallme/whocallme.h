#include <stdio.h>

#define	FUNCTION_NAME_MAXLEN		80

#define who_call_me() \
	do { \
		fprintf(stderr,	"<whocallme>: function <%s> call me <%s>!\n", \
				find_function_by_addr(__builtin_frame_address(0)), who_am_i()); \
	} while(0) 


extern int init_function_table(char *);

