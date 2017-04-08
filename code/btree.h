#ifndef BTREE_H
#define BTREE_H

typedef struct inode {
	unsigned int inum;
	unsigned long size;
	unsigned int filemode;
	time_t created;
	time_t modified;
	//char name[256];  //Depending on our new design revisions, we probably don't need this anymore
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
	unsigned int nextInodeNum;
	node *root;
} btree;

btree *createBTree(int t);
inode *createInode(btree *T);
node *createEmptyNode(int t);
inode *bTreeSearch(btree *T, int uniqueid);
inode *bTreeSearchPath(btree *T, char *path);
void bTreeInsert(btree *T, inode *inode);
void bTreeInsertNonFull(node *node, inode *inode, int t);
void bTreeSplitChild(node *s, int i, node *y, int t);
void deleteNodes(node *n);
void deleteBTree(btree *T);

#endif
