#include<stdio.h>
#include<stdlib.h>

typedef struct Node {
    struct Node* next;
    int data;
} Node;

void insert(Node* list, int data) {
    if(list == NULL) {
        printf("\n first node ..!!");
        list = malloc(sizeof(Node));
        list->next = NULL;
        list->data = data;
    }
    else {
        printf("\n Not a first node ..!!");
        Node* newNode = malloc(sizeof(Node));
        newNode->next = NULL;
        newNode->data = data;
        
        Node* node = list;
        while(node->next != NULL) {
            node = node->next;
        }
        node->next = newNode;
    }
}

void show(Node* list) {
    Node* node = list;
    while(node != NULL) {
        printf("%d ", node->data);
        node = node->next;
    }
}

int main() {
    // Node* list;
    // insert(&list, 10);
    // insert(&list, 20);
    // insert(&list, 30);
    // insert(&list, 40);
    // show(&list);

    char a[10], b[2][10];

    FILE* fp = fopen("text.txt", "r");
    fscanf(fp, "%s,%s", a);
 //   printf("\n a : %s", a[0], a[1]);
    sscanf(a, "%s,%s", b[0], b[1]);


    printf("\n 1 : %s", b[0]);
    printf("\n 2 : %s", b[1]);

    return 0;
}
