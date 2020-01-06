#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student 
{
	int id;
	char name[20];
	float percentage;
};


void func(struct student **record)
{
	struct student *precord = *record;
	printf("func record:%p\n", precord);
	printf(" Id is: %d \n", precord->id);
	printf(" Name is: %s \n", precord->name);
	printf(" Percentage is: %f \n", precord->percentage);
}

int main() 
{
#if 1
	struct student *record = NULL;
	record = (struct student *)malloc(sizeof(struct student));
	record->id=1;
	strcpy(record->name, "Raju");
	record->percentage = 86.5;
	printf("main record:%p\n", record);
#else
	struct student record;

	record.id=1;
	strcpy(record.name, "Raju");
	record.percentage = 86.5;
	printf("main record:%p\n", &record);
#endif
	func(&record);
	return 0;
}


