typedef struct Data {
    char name[10];
    int mis;
    int marks;
} Data;

typedef struct Node {
    struct Node* frount;
    struct Node* back;
    Data* data;
} Node;

typedef Node* DCLL;

void init_DCLL(DCLL* list);
int insert_last_DCLL(DCLL* list, Data* data);
int insert_sorted_DCLL(DCLL* list, Data* data);
void show_DCLL(DCLL* list);
void map(DCLL* list, void (*func)(Data* data));
Data* get_item_DCLL(DCLL* list, int count);
void sort_DCLL(DCLL* list);