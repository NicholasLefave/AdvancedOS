#ifndef BTREE_H
#define BTREE_H

typedef struct inode {
	unsigned int inum;
	unsigned long size;
	unsigned int filemode;
	unsigned int created;
	unsigned int modified;
	char name[256];
	char * data;
} inode;

void inodeInit(inode *node);
int inodeCompare(void *left, void *right);
void freeInode(void *inode);

typedef struct node
{
	int leaf;
	int count;
	inode **keys;
	struct node **ptrs;
}node;

typedef struct btree
{
	int t;
	node *root;
} btree;

btree *createBTree(int t);
inode *createInode(int size, int filemode, char *name);
node *createEmptyNode(int t);
inode *bTreeSearch(btree *T, int uniqueid);
void bTreeInsert(btree *T, inode *inode);
void bTreeInsertNonFull(node *node, inode *inode, int t);
void bTreeSplitChild(node *s, int i, node *y, int t);

#endif
