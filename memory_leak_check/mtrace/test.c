#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mcheck.h>
int main() {
    char *hello;

    setenv("MALLOC_TRACE", "output", 1);
    mtrace();
    if ((hello = (char *) malloc(sizeof(char))) == NULL) {
        perror("Cannot allocate memory.");
        return -1;
    }
	free(hello);

	if ((hello = (char *) malloc(sizeof(char))) == NULL) {
        perror("Cannot allocate memory.");
        return -1;
    }

    return 0;
}
