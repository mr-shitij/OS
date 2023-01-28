#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
	char input[20];
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

		printf("Enter Something ..!!");
		scanf("%s", input);

		if(write(pipe_d[1], input, sizeof(input)) == -1) {
			printf("Error occured during writing to the file ..!!");
			return 2;
		}
		close(pipe_d[1]);
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
