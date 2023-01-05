#include<stdio.h>

int main() {
    int ch;
    FILE* file = fopen("2.png", "r");
    while ((ch = fgetc(file)) != EOF) {
        printf("%c", ch);
    }
    return 0;
}