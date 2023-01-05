#include<stdio.h>

int main(){
    printf("\n Hello Shitij How Is The World ..!!");
    int c;
    while((c = getchar()) != EOF){
        putchar(c);
        c = getchar();
    }

    return 0;
}