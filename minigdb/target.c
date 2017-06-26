#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **arv) {
  char *pmem=NULL;
  for (;;) {
    printf("%d\n", getpid());
	pmem = (char *)malloc(1024);
	printf("pmem=%p\n",pmem);
    sleep(1);
	free(pmem);
  }
  return 0;
}
