#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>

int main(int argc, char* argv[]) {

	if(argc != 3) {
		printf("Invalid Number of arguments requried <device_name> and <inode_number>\n");
		return -1;
	}

	char* device = argv[1];
	int inode_number = atoi(argv[2]);

	int fd;
	int count, ngroups, i;
	struct ext2_super_block superblock;
	struct ext2_group_desc bgdesc[1024];

	fd = open(device, O_RDONLY);
	if(fd < 0) {
		perror("readsuper:");
		exit(errno);
	}

	lseek(fd, 1024, SEEK_CUR);
	count = read(fd, &superblock, sizeof(struct ext2_super_block));

	printf("size of super block - %lu\n", sizeof(struct ext2_super_block));
	printf("Magic: %x\n", superblock.s_magic);
	printf("Inodes Count: %d\n", superblock.s_inodes_count);
	printf("Block size entry: %d\n", superblock.s_log_block_size);
	printf("Size of group descriptor - %lu\n", sizeof(struct ext2_group_desc));
	printf("Blocks count: %d\n", superblock.s_blocks_count);
	printf("Blocks per group: %d\n", superblock.s_blocks_per_group);

	ngroups = ceil(superblock.s_blocks_count / superblock.s_blocks_per_group);
	printf("Total Block Groups: %d\n", ngroups);


	int block_size = 1024 << superblock.s_log_block_size;

	lseek(fd, 1024 + block_size, SEEK_SET); //set pointer to the starting of group descriptor

	count = read(fd, bgdesc, sizeof(struct ext2_group_desc) * ngroups);
	for(i = 0; i < count -1; i++) {
		printf("%d): Inode Table %d\n", i, bgdesc[i].bg_inode_table);
	}

	close(fd);

	return 0;
}

