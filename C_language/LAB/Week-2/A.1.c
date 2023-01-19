#include<stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
	int pd[2], status;
	int ppid = getpid();
        printf("1.Parent : %d\n", ppid);

	pd[0] = fork();

	if(getpid() == ppid) {
		pd[1] = fork();
		if(getpid() == ppid) printf("2.Parent : %d\n", getpid());
	}

	if(getpid() != ppid) {
		printf("Child ID : %d, Parent : %d\n", getpid(), getppid());
		sleep(5);
		//waitpid(getppid(), &status, 0);
		printf("Child Exited ..!!\n");
	}
	else {
		printf("In Parent ..!!\n");
		wait(NULL);
		//waitpid(pd[0], &status, 0);
		//waitpid(pd[1], &status, 0);
		printf("Parent Exit ..!!\n");
	}
	return 0;
}
