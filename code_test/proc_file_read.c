#include <stdio.h>
#include <string.h>

float get_cpu_clock_speed() {
	FILE *fp ;
	char buffer[16384] ;
	size_t bytes_read ;
	char *match ;
	float clock_speed ;

	fp = fopen("/proc/cpuinfo", "r") ;
	bytes_read = fread(buffer, 1, sizeof(buffer), fp) ;
	fclose(fp) ;
	/* Bail if read failed or if buffer isn't big enough */
	if(bytes_read == 0 || bytes_read == sizeof(buffer))
		return 0 ;
	/* NUL-terminate the text */
	buffer[bytes_read] = '\0' ;
	/* Locate the line that starts with "cpu MHz" */
	match = strstr(buffer, "cpu MHz") ;
	if(match == NULL)
		return 0 ;
	/* Parse the line to extract the clock speed */

	sscanf(match, "cpu MHz : %f", &clock_speed) ;
	return clock_speed ;
}

int main() {
	printf("CPU clock speed: %4.2f Mhz\n", get_cpu_clock_speed()) ;
	return 0 ;
}
