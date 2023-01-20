#include<stdio.h>
#include"arguments.h"


int main() {
    Args args;
    init_args(&args, 2, 2);

    add_args(&args, "HA");
    printf("\n.1.\n");

    add_args(&args, "HB");
    printf("\n.2.\n");

    add_args(&args, "HC");
    printf("\n.3.\n");

    add_args(&args, "HD");
    printf("\n.4.\n");

    char** ar = get_agrs(&args);
    for(int i=0; i <= args.len; i++)
        printf("\n Data : %s", args.arguments[i]);

    printf("\n Data 0 : %s", args.arguments[0]);
    return 0;
}