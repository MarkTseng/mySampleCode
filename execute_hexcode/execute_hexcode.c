#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "hello_header.h"

#if 0
int main()
{
   void (*fun)();
   int fd;
   int *map;
   int offset = 8378;
   int pageoffset = offset % getpagesize();
   fd = open("./my_bin", O_RDONLY);
   if(fd == -1) {
        printf("Err opening file\n");
        return -1;
   }
   map = mmap(0, 8378 + pageoffset, PROT_READ|PROT_EXEC,
                        MAP_SHARED, fd, offset - pageoffset);
   perror("Err\n"); //This is printing err and Success!
   //fun = map; // If I uncomment this and 
   //fun();     // this line then, still it 
                // print err and Success! from perror
                // but later it says Illegal instruction. 
   return 0;
}
#endif

int main ()
{
	void *myBinCode;

	off_t offset, pa_offset;
	offset = sizeof(helloworld_bin);
	pa_offset = offset & ~(getpagesize() - 1);

	printf("offset: %d, pa_ffset:%d , pagesize:%d\n", offset, pa_offset, getpagesize());

	/* copy helloworld_bin to executable buffer */    
	myBinCode= mmap (0,offset*2 - pa_offset,PROT_READ|PROT_WRITE|PROT_EXEC,
			MAP_PRIVATE|MAP_ANON,-1,pa_offset);
	memcpy (myBinCode, helloworld_bin, sizeof(helloworld_bin));

	/* run code */
	((void (*) (void))myBinCode)();
}

