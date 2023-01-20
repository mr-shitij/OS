#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    int fd = open("/media/shitij_agrawal/SHARED_DATA/Notes_COEP/6thSem/OS/C_language/assignments/file_descriptors/temp.txt", O_WRONLY);

    int temp_output = dup(1);
    close(1);

    dup(fd);

    printf("\n HELLO");
    return 0;
}