#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/capability.h>
#include <errno.h>
#include <sys/prctl.h>

int main()
{
     struct __user_cap_header_struct cap_header_data;
     cap_user_header_t cap_header = &cap_header_data;

     struct __user_cap_data_struct cap_data_data;
     cap_user_data_t cap_data = &cap_data_data;

	
     cap_header->pid = getpid();
     cap_header->version = _LINUX_CAPABILITY_VERSION_1;
 
     if (capget(cap_header, cap_data) < 0) {
         perror("Failed capget");
         exit(1);
     }
     printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective,
         cap_data->permitted, cap_data->inheritable);

     /* Clear all but the capability to bind to low ports */
     cap_data->effective |= CAP_SYS_ADMIN;
     cap_data->permitted |= CAP_SYS_ADMIN;
     cap_data->inheritable |= CAP_SYS_ADMIN;

	 if (capset(cap_header, cap_data) < 0)
		 printf("capset failed: %m");

	 printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective,
			 cap_data->permitted, cap_data->inheritable);


	 /* Tell kernel not clear capabilities when dropping root */
	 if (prctl(PR_SET_KEEPCAPS, 1) < 0)
		 printf("prctl(PR_SET_KEEPCAPS) failed: %m");

		setresuid(1000,1000,1000);
	 /* Now we can drop privilege, but retain CAP_NET_BIND_SERVICE */
	 if (setuid(getuid()) < 0)
		 printf("unable to drop privs: %m");

	while(1)
	{
		sleep(1);
	}

}

