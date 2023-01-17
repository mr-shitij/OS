#include"DCLL.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void show_data(Data* data) {
    printf("Name: %s, Mis: %d, Marks: %d \n", data->name, data->mis, data->marks);
}

void init_DCLL(DCLL* list) {
    *list = NULL;
}

int insert_last_DCLL(DCLL* list, Data* data) {
    Node* new_node = malloc(sizeof(Node));
    new_node->frount = NULL;
    new_node->back = NULL;

    new_node->data = malloc(sizeof(Data));
    new_node->data->mis = data->mis;
    new_node->data->marks = data->marks;
    strcpy(new_node->data->name, data->name);

    if(*list == NULL){
        new_node->frount = new_node;
        new_node->back = new_node;
        *list = new_node;
    }
    else {
        Node* node = *list;
        while(node->frount != *list) {
            node = node->frount;
        }
        new_node->frount = *list;
        new_node->back = node;

        (*list)->back = new_node;

        node->frount = new_node;
    }
}

int insert_sorted_DCLL(DCLL* list, Data* data) {
    Node* new_node = malloc(sizeof(Node));
    new_node->frount = NULL;
    new_node->back = NULL;

    new_node->data = malloc(sizeof(Data));
    new_node->data->mis = data->mis;
    new_node->data->marks = data->marks;
    strcpy(new_node->data->name, data->name);

    if(*list == NULL) {
        new_node->frount = new_node;
        new_node->back = new_node;
        *list = new_node;
    }
    else {
        Node* node = *list;
        while(node->frount != *list) {
            if(node->data->mis > data->mis)
                break;
            node = node->frount;
        }
        if(node->data->mis > data->mis) {
            new_node->back = node->back;
            new_node->back->frount = new_node;
            new_node->frount = node;

            node->back = new_node;

            if(node == *list) {
                *list = new_node;
            }
        }
        else{
            new_node->frount = *list;
            new_node->back = node;

            (*list)->back = new_node;

            node->frount = new_node;
        }
    }
}

void show_DCLL(DCLL* list) {
    if(*list == NULL)
        return;

    Node* node = *list;
    do {
        show_data(node->data);
        node = node->frount;
    }
    while(node->frount != *list);
    
    if(*list != node)
        show_data(node->data);

}

Data* get_item_DCLL(DCLL* list, int count){
    if(*list == NULL)
        return NULL;

    Node* node = *list;
    while(node->frount != *list && count != 0){
        node = node->frount;
        count--;
    }

    if(count == 0)
        return node->data;

    return NULL;
}

void map(DCLL* list, void (*func)(Data* data)) {
    if(*list == NULL)
        return;

    Node* node = *list;
    do {
        func(node->data);
        node = node->frount;
    }
    while(node->frount != *list);
    
    if(*list != node)
        func(node->data);
}

void swap_Node(Node* first, Node* second) {
    Data* temp = first->data;
    first->data = second->data;
    second->data = temp;
}

void sort_DCLL(DCLL* list) {
    if(list == NULL)
        return;

    Node* node = *list;
    do
    {
        Node* next = node->frount;
        do
        {
            if(node->data->mis > next->data->mis) {
                swap_Node(node, next);
            }
            next = next->frount;
        } while (next != *list);
        node = node->frount;
    } while (node->frount != *list);
}
