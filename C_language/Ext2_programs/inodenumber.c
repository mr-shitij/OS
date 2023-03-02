#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>
#include <sys/types.h>
#include <sys/stat.h>


#define BLOCK_SIZE 1024


ino_t get_inode(char *path) {
    struct stat buf;

    if (stat(path, &buf) == -1) {
        perror("stat");
        exit(1);
    }

    return buf.st_ino;
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <device> <file_name> <inode/data>\n", argv[0]);
		return 1;
	}

	char *device = argv[1];
	int inode_number = get_inode(argv[2]);

	int is_inode_data = 0;

	if(strcmp("inode", argv[3]) == 0) {
		is_inode_data = 1;
	}
	else if(strcmp("data", argv[3]) == 0) {
		is_inode_data = 2;
	}


	int fd = open(device, O_RDONLY);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	struct ext2_super_block superblock;
	lseek(fd, 1024, SEEK_SET);
	if (read(fd, &superblock, sizeof(superblock)) != sizeof(superblock)) {
		perror("pread");
		return 1;
	}

	int block_size = BLOCK_SIZE << superblock.s_log_block_size;

	struct ext2_group_desc groupdesc;
	int block_group = (inode_number - 1) / superblock.s_inodes_per_group;

	lseek(fd, 1024 + block_size + (block_group * sizeof(struct ext2_group_desc)), SEEK_SET);
	if (read(fd, &groupdesc, sizeof(groupdesc)) != sizeof(groupdesc)) {
		perror("pread");
		return 1;
	}

	int inode_table_block = groupdesc.bg_inode_table;
	int inode_table_offset = (inode_number - 1) % superblock.s_inodes_per_group;
	int inode_offset = inode_table_offset * sizeof(struct ext2_inode);

	int inode_block = inode_table_block + (inode_offset / block_size);
	int inode_block_offset = inode_offset % block_size;


	char buf[sizeof(struct ext2_inode)];
	lseek(fd, inode_table_block * block_size + inode_table_offset, SEEK_SET);
	read(fd, buf, sizeof(buf));

	struct ext2_inode *inode = (struct ext2_inode *)buf;

	if(is_inode_data == 1) {
		printf("Inode information:\n");
		printf("mode: %x\n", inode->i_mode);
		printf("uid: %u\n", inode->i_uid);
		printf("gid: %u\n", inode->i_gid);
		printf("size: %u\n", inode->i_size);
		printf("atime: %u\n", inode->i_atime);
		printf("ctime: %u\n", inode->i_ctime);
		printf("mtime: %u\n", inode->i_mtime);
		printf("dtime: %u\n", inode->i_dtime);
		printf("link count: %u\n", inode->i_links_count);
		printf("blocks: %u\n", inode->i_blocks);
		printf("flags: %u\n", inode->i_flags);
		printf("File size: %d bytes\n", inode->i_size);
	}
	else if(is_inode_data == 2) {
		int i;
		char block_buf[block_size];
		for (i = 0; i < 12 && inode->i_block[i]; i++) {
		    off_t block_offset = inode->i_block[i] * block_size;
		    lseek(fd, block_offset, SEEK_SET);
		    read(fd, block_buf, block_size);
		    fwrite(block_buf, 1, block_size, stdout);
		}
	}

    return 0;
}

