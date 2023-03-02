#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <error.h>
#include <sys/types.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>
#include <errno.h>

void printSuperBlock(struct ext2_super_block sb);
void printBlockGroupDescriptors(int fd, struct ext2_super_block sb, int ngroups);
void printInodeInfo(int fd, struct ext2_super_block sb, int inode_num);
void readExt2Filesystem(int fd, int inode_num);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf(";)\n");
        printf("Usage: %s <ext2-image>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);

    if (fd < 0)
    {
        perror("Error opening the file\n");
        exit(1);
    }

    readExt2Filesystem(fd, atoi(argv[2]));

    close(fd);
    return 0;
}

void printSuperBlock(struct ext2_super_block sb)
{
    printf("#Magic-No: %x\n", sb.s_magic);
    printf("#Inodes-Count: %d\n", sb.s_inodes_count);
    printf("#Block-Size-Entry: %d\n", sb.s_log_block_size);
}

void printBlockGroupDescriptors(int fd, struct ext2_super_block sb, int ngroups)
{
    struct ext2_group_desc bgdesc[1024];
    int count, i;

    lseek(fd, 1024 + 1024 << sb.s_log_block_size, SEEK_SET);

    count = read(fd, bgdesc, sizeof(struct ext2_group_desc) * ngroups);

    for (i = 0; i < ngroups; i++)
    {
        printf("%d : #Inode-Table: %d\n", i, bgdesc[i].bg_inode_table);
    }
}

void printInodeInfo(int fd, struct ext2_super_block sb, int inode_num)
{
    int inode_size = sb.s_inode_size;
    int inodes_per_group = sb.s_inodes_per_group;
    int inode_table_block = 0;
    int inode_index = 0;

    if (inode_num < 1 || inode_num > sb.s_inodes_count)
    {
        printf("#Invalid-inode-number\n");
        return;
    }

    inode_num -= 1;
    inode_table_block = sb.s_first_data_block + sb.s_blocks_per_group;
    inode_index = inode_num % inodes_per_group;

    lseek(fd, (inode_table_block * sb.s_log_block_size) + (inode_index * inode_size), SEEK_SET);

    struct ext2_inode inode;
    read(fd, &inode, sizeof(struct ext2_inode));

    printf("# Inode-Information-for-inode-number %d:\n", inode_num + 1);
    printf("# Mode-is : %x\n", inode.i_mode);
    printf("# UID-is : %d\n", inode.i_uid);
    printf("# GID-is : %d\n", inode.i_gid);
    printf("# Size-is : %d\n", inode.i_size);
    printf("# Blocks-are : %d\n", inode.i_blocks);
    printf("# Creation-Time-is : %d\n", inode.i_ctime);
    printf("# Access-Time-is : %d\n", inode.i_atime);
    printf("# Modification-Time-is : %d\n", inode.i_mtime);
    printf("# Links-Count-is : %d\n", inode.i_links_count);
    printf("# Flags-are : %d\n", inode.i_flags);
    printf("# Direct-Blocks-are : ");

    for (int i = 0; i < 12; i++)
    {
        printf("%d ", inode.i_block[i]);
    }
    printf("\n");

    if (inode.i_block[12] != 0)
    {
        int block_size = 1024 << sb.s_log_block_size;
        int blocks[1024];
        lseek(fd, inode.i_block[12] * block_size, SEEK_SET);
        read(fd, &blocks, block_size);
        printf("In-direct blocks: ");
        for (int i = 0; i < block_size / 4; i++)
        {
            if (blocks[i] != 0)
            {
                printf("%d ", blocks[i]);
            }
        }
        printf("\n");
    }
}

void readExt2Filesystem(int fd, int inode_num)
{
    struct ext2_super_block sb;
    
    if (pread(fd, &sb, sizeof(sb), 1024) != sizeof(sb))
    {
        perror("Unable to read superblock\n");
        exit(1);
    }

    if (sb.s_magic != EXT2_SUPER_MAGIC)
    {
        printf("Not an 'ext2' File-system\n");
        exit(1);
    }

    int block_size = 1024 << sb.s_log_block_size;
    int ngroups = (sb.s_blocks_count + sb.s_blocks_per_group - 1) / sb.s_blocks_per_group;
    printf("Number of Block Groups : %d\n", ngroups);

    printInodeInfo(fd, sb, inode_num);

    // printSuperBlock(sb);
    // printBlockGroupDescriptors(fd, sb, ngroups);

}