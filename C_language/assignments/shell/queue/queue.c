#include<stdio.h>
#include<stdlib.h>
#include"queue.h"

void queue_init(queue_list *list) {
	*list = NULL;
}

void queue_enqueue(queue_list *list, void* data) { // Insert At Last
	queue_node* new_node = malloc(sizeof(queue_node));	
	new_node->next = NULL;
	new_node->data = data;
	
	if(new_node == NULL) {
		return;
	}

	if(*list  == NULL) {
		*list = new_node;
	} else {
		queue_node *last_node = *list;
		while(last_node->next != NULL) {
			last_node = last_node->next;
		}
		last_node->next = new_node;
	}
}

int queue_dequeue(queue_list *list) { // Remove From First
	if(*list == NULL) {
		return 1; // queue underflow
	} else {
		queue_node* second_node = NULL;
		
		if((*list)->next != NULL)
			second_node = (*list)->next;
		
		free((*list)->data);
		free(*list);
		*list = second_node;		
	}
	return 0;
}

