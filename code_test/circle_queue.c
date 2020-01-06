#include <stdio.h>
#include <stdlib.h>
#define MAX_QUEUE 5
char CircularQueue[MAX_QUEUE];
int front = -1, rear = -1;
int flag = 0; //改良原本只能存放N-1個元素，加上此布林變數即可存放N個元素

int isFull(){
    return (rear % MAX_QUEUE == front);
}

int isEmpty(){
    return front == rear;
}

void Add(char* queue, char item){
    if (isFull() && flag == 1 || rear == MAX_QUEUE - 1 && front == -1) {
        printf("Circular Queue is full!\n");
        return;
    }
    printf("Circular Queue add: %c\n", item);
    rear = (rear + 1) % MAX_QUEUE;
    queue[rear] = item;
    if (front == rear) flag = 1;
}

void Delete(char* queue){
    if (isEmpty() && flag == 0){
        printf("Circular Queue is empty!\n");
        return;
    }
    front = (front + 1) % MAX_QUEUE;
    printf("%c is deleted.\n", queue[front]);
    queue[front] = 0;
    if (front == rear) flag = 0;
}

void printQueue(char* queue){
    if (isEmpty() && flag == 0){
        printf("Circular Queue is empty!\n");
        return;
    }
    printf("Circular Queue: ");
    for (int i = 0; i < MAX_QUEUE; i++)
        printf("%c ", queue[i]);
    printf("\n\n");
}

int main(){
    Add(CircularQueue, '1');
    Add(CircularQueue, '2');
    Add(CircularQueue, '3');
    Add(CircularQueue, '4');
    Add(CircularQueue, '5');
    Add(CircularQueue, '6'); //shows "Circular Queue is full!"
    Delete(CircularQueue);
    Delete(CircularQueue);
    Delete(CircularQueue);
    printQueue(CircularQueue);
    Add(CircularQueue, '6');
    printQueue(CircularQueue);
    getchar();
    return 0;
}
