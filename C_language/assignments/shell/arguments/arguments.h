typedef struct Arguments {
    int buffer_growth_rate; // rate at which buffer should grow when got full
    int buffer; // the size of actual array
    int len; // the total length of data inside array ie. index of last element
    char** arguments;
}Arguments;

typedef Arguments Args;

int init_args(Args *args, int buffer_size, int buffer_growth_rate);
int set_buffer_args(Args *args, int buffer_size);
void add_args(Args *args, char* str);
char** get_agrs(Args *args);
