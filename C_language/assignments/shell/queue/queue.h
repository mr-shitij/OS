#include<stdio.h>
#include<stdlib.h>

typedef struct queue_node {
	struct queue_node* next;
	int data;
}queue_node;
typedef queue_node* queue_list;

void queue_init(queue_list *list);

// To work as a queue
void queue_enqueue(queue_list *list, int data);
int queue_dequeue(queue_list *list);

