#include"arguments.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int init_args(Args *args, int buffer_size, int buffer_growth_rate) {
    if(buffer_size <= 1)
        return -1;
    args->buffer_growth_rate = buffer_growth_rate;
    args->buffer = buffer_size;
    args->len = -1;
    args->arguments = malloc(sizeof(char*) * args->buffer);
    args->arguments[0] = NULL;
    return 0;
}

int set_buffer_args(Args *args, int buffer_size) {
    if(buffer_size < args->len)
        return -1; // as data may get lost

    char** new_buffer = malloc(sizeof(char) * buffer_size);
    for(int i = 0; i <= args->len; i++) {
        new_buffer[i] = malloc(sizeof(char) * strlen(args->arguments[i]));
        strcpy(new_buffer[i], args->arguments[i]);
        printf("\nHIII : %s\n", args->arguments[i]);
        free(args->arguments[i]);
        printf("\nHELLO : %s\n", args->arguments[i]);
    }
    free(args->arguments);
    args->buffer = buffer_size;
    args->arguments = new_buffer;
    return 0;
}

void add_args(Args *args, char* str) {
    // printf("Before Add Args : \n");
    // printf("0. Data 0 : %s\n", args->arguments[0]);
    // printf("1. Buffer: %d\n", args->buffer);
    // printf("2. Len : %d\n", args->len);
    
    // printf("0.0. Data 0 : %s\n", args->arguments[0]);
    if(args->len + 2 >= args->buffer) // 1 is reserved for the NULL so check for two
        printf("New Buffer Allocated : %d\n", set_buffer_args(args, args->buffer + args->buffer_growth_rate));
    // printf("0.1. Data 0 : %s\n", args->arguments[0]);
    
    args->arguments[++(args->len)] = malloc(sizeof(char) * strlen(str));
    // printf("0.2. Data 0 : %s\n", args->arguments[0]);
    strcpy(args->arguments[args->len], str);
    // printf("0.3. Data 0 : %s\n", args->arguments[0]);


    // printf("0.3.5.0 : LEN : %d\n", args->len);
    printf("0.3.5.1 : BUF : %d\n", args->buffer);
    // printf("0.3.5.2 : DAT : %s\n", args->arguments[args->len]);

    // printf("0.8.8. Data 0 : %s\n", args->arguments[0]);

    args->arguments[args->len + 1] = NULL;

    // printf("0.3.6.0 : LEN : %d\n", args->len);
    // printf("0.3.6.1 : BUF : %d\n", args->buffer);
    // printf("0.3.6.2 : DAT : %s\n", args->arguments[args->len + 1]);

    // printf("0.3.5. Data 0 : %s\n", args->arguments[0]);
    // printf("After Add Args : \n");
    // printf("0.4. Data 0 : %s\n", args->arguments[0]);
    // printf("1. Buffer: %d\n", args->buffer);
    // printf("2. Len : %d\n", args->len);
}

char** get_agrs(Args *args) {
    return args->arguments;
}
