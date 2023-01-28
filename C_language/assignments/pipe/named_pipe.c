#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int main() {
	char input[20];
	int pipe_d[2]; // 0 = read & 1 = write

	if(mkfifo("MyFIFO", 0777) == -1) {
		if(errno != EEXIST) {
			printf("Could not create fifo file\n");
			return 1;
		}
	}

	printf("Openning ..!!\n");
	int fd = open("MyFIFO", O_WRONLY);
	printf("Opened ..!!\n");

	int x = 'Z';
	if(write(fd, &x, sizeof(x)) == -1) {
		return 2;
	}
	printf("Written ..!!\n");
	close(fd);
	printf("Closed ..!!\n");
	return 0;
}
