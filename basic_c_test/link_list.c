#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct phone_list *phoneHead_0910 = NULL;
struct phone_list *phoneHead_0933 = NULL;
struct phone_list *phoneHead_0970 = NULL;
struct phone_list *phoneHead_0988 = NULL;

enum NODE_STATUS
{
	ADD_NODE_SUCCESS,
};

typedef struct phone_list {
	char phoneNumber[10];
	struct phone_list *next;
}phone_list;

int addNode(char *phoneNumber)
{
	struct phone_list **phonePtr;

	if(!strncmp(phoneNumber,"0910",4))
	{
		phonePtr = &phoneHead_0910;
	}
	else if(!strncmp(phoneNumber,"0933",4))
	{
		phonePtr = &phoneHead_0933;
	}
	else if(!strncmp(phoneNumber,"0970",4))
	{
		phonePtr = &phoneHead_0970;
	}
	else if(!strncmp(phoneNumber,"0988",4))
	{
		phonePtr = &phoneHead_0988;
	}
	else
	{
		printf("error group\n");
		exit(-20);
	}


	phone_list *tmpNode;
	tmpNode = malloc(sizeof(phone_list));
	strncpy(tmpNode->phoneNumber, phoneNumber, 10);
	tmpNode->next = *phonePtr;

	printf("add node: %p \n",tmpNode);
	*phonePtr = tmpNode;
	printf("0988 %p \n",*phonePtr);

	return ADD_NODE_SUCCESS;
}

void freeList(phone_list *HeadNode)
{
	struct phone_list *phonePtr;
	phonePtr = HeadNode;
	while(phonePtr != NULL)
	{
		printf("Free node ptr: %p\n",phonePtr);
		phonePtr = phonePtr->next;
		free(phonePtr);
	}
}

void showList(phone_list *HeadNode)
{
	struct phone_list *phonePtr;
	phonePtr = HeadNode;
	while(phonePtr != NULL)
	{
		printf("phone number: %s\n",phonePtr->phoneNumber);
		printf("Node ptr: %p\n",phonePtr);
		phonePtr = phonePtr->next;
	}


}

#define GET_PHONE_ARRAY_SIZE(array) (sizeof(array)/sizeof(char *))

int main()
{
	char *phoneArray[] = {"0910100100", "0910123011", "0910200333",
						"0933100100", "0933123011", "0933200333",
						"0970333101", "0970777001", "0970888888",
						"0988001001", "0988555666", "0988888888"};
	int i,numberOfPhone;

	numberOfPhone = GET_PHONE_ARRAY_SIZE(phoneArray);

	for(i=0;i<numberOfPhone;i++)
	{
		printf("Add phone numer %s to list\n", *(phoneArray + i));
		addNode(*(phoneArray + i));
	}

	showList(phoneHead_0910);
	showList(phoneHead_0933);
	showList(phoneHead_0970);
	showList(phoneHead_0988);

	freeList(phoneHead_0910);
	freeList(phoneHead_0933);
	freeList(phoneHead_0970);
	freeList(phoneHead_0988);

	return 0;
}
