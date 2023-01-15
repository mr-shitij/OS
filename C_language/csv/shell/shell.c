#include<stdio.h>

int main() {
    // execl("/bin/ls", "ls", "-l", NULL);
    char command[1024] = "exit";
    do {
        printf("prompt>");
        scanf("%s", command);
        int pid = fork();
        if(pid == 0) {
            char arguments[1024];

            execl(command, command, NULL);
        } else {
            wait(0);
        }
    } while (strcmp(command, "exit"));    
    return 0;
}