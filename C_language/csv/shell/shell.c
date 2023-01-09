#include<stdio.h>

int main() {
    char command[30] = "exit";
    do {
        printf("prompt>");
        scanf("%s", command);
        int pid = fork();
        if(pid == 0) {
            execl(command, command, NULL);
        } else {
            wait(0);
        }
    } while (strcmp(command, "exit"));    
    return 0;
}