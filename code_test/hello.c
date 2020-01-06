/*******************************************
* File Name : hello.c
* Purpose :
* Creation Date : 23-09-2016
* Last Modified : Fri 23 Sep 2016 10:30:57 AM CST
* Created By : Mark Tseng  
**********************************************/

#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

int main() {
    // Declare a variable named 'input_string' to hold our input.
    char input_string[105]; 
    
    // Read a full line of input from stdin and save it to our variable, input_string.
    scanf("%[^\n]s", input_string); 
    
    // Print a string literal saying "Hello, World." to stdout using printf.
    printf("Hello, World.\n");
    
    // TODO: Write a line of code here that prints the contents of input_string to stdout.
    printf("%s\n",input_string);
    return 0;
}
