/* issue an error message via perror() and terminate the program */
void die(char * message);

/* Copies data from file descriptor 'from' to file descriptor
   'to' until nothing is left to be copied. Exits if an error
   occurs. This assumes both from and to are set for blocking
   reads and writes. */
void copyData(int from, int to);
 
