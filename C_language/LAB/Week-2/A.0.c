#include<stdio.h>
#include<unistd.h>
int main() {
	int i = execl("/root", "root", NULL);
	printf("\n %d", i);
	return i;
}
