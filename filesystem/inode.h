#ifndef MY_INODE
#define MY_INODE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "btree.h"

/* pointer to inline function? */
typedef struct inode {
	unsigned int inum;
	unsigned long size;
	unsigned int filemode;
	unsigned int created;
	unsigned int modified;
	byte * data;
} inode;

void inodeInit(inode *node);
int inodeCompare(void *left, void *right);
void freeInode(void *inode);

#endif
