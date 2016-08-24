#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void main(){
	int num;
	printf("enter number");
	scanf("%d",&num);
	while(num>=9){
		num=num-9;
	}
	if(num==0||num==3||num==6){
		printf("Given Number is divisible by 3\n");
	}else{
		printf("Given Number is not divisible by 3\n");
	}
}
