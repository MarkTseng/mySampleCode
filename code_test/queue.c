#include <stdio.h>
#include <stdlib.h>
#define MAX_QUEUE 5
char Queue[MAX_QUEUE];
int front = 0, rear = 0;

int isFull(){
	return rear == MAX_QUEUE && front == 0;
}

int isEmpty(){
	return front == rear;
}

void Add(char* queue, char item){
	if (isFull()) {
		printf("Queue is full!\n");
		return;
	}
	printf("%c added.\n", item);
	queue[rear++] = item;
}

void Delete(char* queue){
	if (isEmpty()){
		printf("Queue is empty!\n");
		return;
	}
	printf("%c is deleted.\n", queue[front]);
	queue[front++] == ' ';
}

void printQueue(char* queue){
	if (isEmpty()){
		printf("Queue is empty!\n");
		return;
	}
	printf("Queue: ");
	for (int i = front; i < rear; i++)
		printf("%c ", queue[i]);
	printf("\n");
}

int main(){
	Add(Queue, '1');
	Add(Queue, '2');
	Add(Queue, '3');
	Add(Queue, '4');
	Add(Queue, '5');
	Add(Queue, '6'); //shows "Queue is full!"
	printQueue(Queue); //shows "1 2 3 4 5"
	Delete(Queue);
	printQueue(Queue); //shows "2 3 4 5"
	Delete(Queue);
	Delete(Queue);
	Delete(Queue);
	Delete(Queue);
	Delete(Queue);
	Delete(Queue); //shows "Queue is empty!"

	getchar();
	return 0;
}
