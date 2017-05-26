// Jesse Kennedy
// CPSC 322 Project 1
// Fall 2015

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int arglen = 0;
  for (int i = 1; i < argc; i++)
    arglen += strlen(argv[i]) + 1;  // Length of arguments plus a space

  const char *preload = "LD_PRELOAD=./memory_shim.so";
  arglen += strlen(preload) + 1;  // Length of preload literal plus a \n

  char data[arglen];
  strcpy(data, preload);

  for (int i = 1; i < argc; i++) {
    strcat(data, " ");
    strcat(data, argv[i]);
  }
  printf("%s\n", data);

  FILE *pipe = popen(data, "r");

  if (pipe == NULL)
    perror("Error opening piped file.");
  
  pclose(pipe);
}
