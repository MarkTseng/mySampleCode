#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int reverse(int i);
	char st[]="Hello World";
	void main(){
	printf("\nThe string is: %s", st);
	reverse(0);
	printf("\nReversed string is: %s", st);
	
}
int reverse(int i){
	if(i<(strlen(st)/2)){
	char c;
	c= st[i];
	st[i]=st[strlen(st)-i-1];
	st[strlen(st)-i-1]=c;
	reverse(i+1);
	}
	return 0;
}