#include<stdio.h>
#include<stdlib.h>

typedef struct stack_node {
	struct stack_node* next;
	int data;
}stack_node;
typedef stack_node* stack_list;

void stack_init(stack_list *list);

// To work as a stack
void stack_print(stack_list list);
void stack_push(stack_list *list, int data);
int stack_pop(stack_list *list);
int stack_peek(stack_list list);