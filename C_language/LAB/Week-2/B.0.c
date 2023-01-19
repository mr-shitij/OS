#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	int fd = open("./temp.txt", O_WRONLY);
	int output_fd = dup(1);

	close(1);

	dup(fd);

	printf("HELLO IN FILE");

	return 0;
}
