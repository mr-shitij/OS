#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	char input[200];
	int pipe_d[2]; // 0 = read & 1 = write

	if(pipe(pipe_d) == -1) {
		printf("An error occured during opennig of file\n");
		return 1;
	}

	int pd = fork();
	if(pd < 0) {
		printf("Not able to create child ..!!\n");
	} else if(pd == 0) {
		printf("Inside of child ..!!\n");
		close(pipe_d[0]);
        
        close(1); // removing the standard output
        dup(pipe_d[1]); // adding to the standard output
        close(pipe_d[1]); // removing redundant

        execl("/bin/ls", "ls", NULL);

	} else {
        printf("In Parent ..!!\n");
		close(pipe_d[1]);
		if(read(pipe_d[0], input, sizeof(input)) == -1) {
			printf("Error Occured During reading from file ..!!");
			return 3;
		}
		printf("Recived From Child : %s", input);
		close(pipe_d[0]);
	}
	return 0;
}
