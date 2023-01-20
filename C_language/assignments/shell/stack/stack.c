#include<stdio.h>
#include<stdlib.h>
#include"stack.h"

void stack_init(stack_list *list) {
	*list = NULL;
}

int stack_pop(stack_list *list) { // Remove From First
	if(*list == NULL) {
		return 1; // queue underflow
	} else {
		stack_node* second_node = NULL;
		
		if((*list)->next != NULL)
			second_node = (*list)->next;
		
		free(*list);
		*list = second_node;		
	}
	return 0;
}

void stack_push(stack_list *list, int data) { // Insert At First
	stack_node* new_node = malloc(sizeof(stack_node));	
	new_node->next = NULL;
	new_node->data = data;

	if(new_node == NULL) {
		return;
	}

	if(*list == NULL) {
		*list = new_node;
	} else {
		stack_node *first_node = *list;
		*list = new_node;
		(*list)->next = first_node;		
	}
}

int stack_peek(stack_list list) {
	if(list == NULL)
		return -1;
	return list->data;
}

void stack_print(stack_list list) {
	if(list == NULL)
		return;

	stack_node* node = list;
	while(node != NULL) {
		printf("%d\n", node->data);
		node = node->next;
	}
}
