#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>

void handle_sigtstp(int sig) {
    printf("PID In FUNC : %d \n\n\n\n", getpid());
    // kill(getpid(), SIGSTOP);
}

int main() {
    // signal(SIGTSTP, handle_sigtstp);
    printf("Parent: %d\n", getpid());
    int pid = fork();
    if(pid == 0) {
        printf("Child: %d\n", getpid());
        signal(SIGTSTP, handle_sigtstp);
        int d = fork();
        if(d == 0)
           execl("/bin/sleep", "sleep", "30", NULL);
        else   
            wait(NULL);
    }
    else {
        int status;
        waitpid(pid, &status, WUNTRACED);
        printf("PROCESS EXITED\n\n\n\n");
        if(WIFSTOPPED(status)) {
            printf("STOP SIGNLA\n\n\n\n");
        }
    }
    return 0;
}

/*
    struct sigaction sa;
    sa.sa_handler = &handle_sigtstp;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa, NULL);
*/