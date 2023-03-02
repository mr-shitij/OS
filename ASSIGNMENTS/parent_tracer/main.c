#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char command[1024];
    int pid;

    int pipe_id[2];

    printf("\n Enter the pid : ");
    scanf("%d", &pid);
    while (pid != 0)
    {
        pipe(pipe_id);
    
        char str_pid[10];
        int t_pid = fork();
        if(t_pid == 0) {
            close(pipe_id[0]);
            close(1);
            dup(pipe_id[1]);

            sprintf(str_pid, "%d", pid);

            strcpy(command, "cat /proc/");
            strcat(command, str_pid);
            strcat(command, "/status | grep PPid | cut -d \":\" -f 2 | sed -e 's/^[[:space:]]*//'");
            system(command);

            close(pipe_id[1]);
            return;
        } else if(t_pid == -1) {
            perror("\n Unable to fork");
        } else {
            close(pipe_id[1]);

            read(pipe_id[0], str_pid, sizeof(str_pid));
            pid = atoi(str_pid);

            printf("\n PID : %d", pid);

            close(pipe_id[0]);
            wait(0);
        }
    }
    
    return 0;
}

// cat /proc/281/status | grep PPid | cut -d : -f 2 | sed -e 's/^[[:space:]]*//'
