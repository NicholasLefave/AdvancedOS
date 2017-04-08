#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "btree.h"

btree *createBTree(int t)
{
	btree *bt = (btree *)malloc(sizeof(btree));
	bt->t = t;
	bt->nextInodeNum = 0;
	bt->root = createEmptyNode(t);
	
	return bt;
}

inode *createInode(btree *T)
{
	inode *in = (inode *)malloc(sizeof(inode));
	in->inum = T->nextInodeNum++;
	in->size = 0;
	in->filemode = -1;
	in->created = time(0);
	in->modified = time(0);
	in->data = NULL;

	bTreeInsert(T, in);

	return in;
}

node *createEmptyNode(int t)
{
	node *n = (node *)malloc(sizeof(node));
	n->leaf = 1;
	n->count = 0;
	n->keys  = (inode **)malloc((2*t-1) * sizeof(inode *));
	n->ptrs = (node **)malloc((2*t) * sizeof(node *));

	int x;
	for (x = 0; x < (2*t-1); x++)
		n->keys[x] = NULL;

	for (x = 0; x < (2*t); x++)
		n->ptrs[x] = NULL;

	return n;
}

inode *bTreeSearch(btree *T, int uniqueid)
{
	node *cur = T->root;

	while (cur != NULL)
	{
		int i;
		int count = cur->count;
		for (i = 0; i < count; i++)
		{
			if (cur->keys[i]->inum == uniqueid)
			{
				return cur->keys[i];
			}
			else if (cur->keys[i]->inum > uniqueid)
			{
				cur = cur->ptrs[i];
				break;
			}
			else if (i + 1 == cur->count)
			{
				cur = cur->ptrs[cur->count];
			}
		}
	}

	return NULL;
}

inode *bTreeSearchPath(btree *T, char *path)
{
	inode* target = bTreeSearch(T, 0);
	char *pathCopy = (char *)calloc(strlen(path)+1, sizeof(char));
	strcpy(pathCopy, path);
	char* dataCopy = NULL;
	char* dataToken = NULL;
	char *prevSlash = strchr(pathCopy, '/');
	char *currSlash = strchr(prevSlash+1, '/');

	if (!strcmp(prevSlash, "/"))
	{
		free(pathCopy);
		return target;
	}
	
	while (prevSlash != NULL)
	{
		char *file = NULL;
		
		if (currSlash == NULL)
		{
			file = (char *)calloc(strlen(prevSlash), sizeof(char));
			strncpy(file, prevSlash+1, strlen(prevSlash)-1);
		}
		else
		{
			file = (char *)calloc(currSlash - prevSlash, sizeof(char));
			strncpy(file, prevSlash+1, currSlash - prevSlash - 1);
		}

		if (!(target->filemode & 01000))
		{ 
			free(pathCopy);
			return NULL;
		}
		
		dataCopy = (char *)calloc(strlen(target->data)+1, sizeof(char));
		strcpy(dataCopy, target->data);
		dataToken = strtok(dataCopy, "#\n");
		while(dataToken != NULL && strcmp(file, dataToken) != 0)
			dataToken = strtok(NULL, "#\n");
		
		if (dataToken == NULL)
		{ 
			free(pathCopy);
			return NULL;
		}

		dataToken = strtok(NULL, "#\n");
		target = bTreeSearch(T, atoi(dataToken));

		prevSlash = currSlash;
		if (currSlash != NULL)
			currSlash = strchr(prevSlash+1, '/');
		free(dataCopy);
		free(file);
	}
	free(pathCopy);
	return target;
}

void bTreeInsert(btree *T, inode *inode)
{
	node *r = T->root;
	if (r->count >= (2*T->t-1))
	{
		node *s = createEmptyNode(T->t);
		T->root = s;
		s->leaf = 0;
		s->ptrs[0] = r;
		bTreeSplitChild(s, 1, r, T->t);
		bTreeInsertNonFull(s, inode, T->t);
	}
	else
	{
		bTreeInsertNonFull(r, inode, T->t);
	}
}

void bTreeInsertNonFull(node *x, inode *inode, int t)
{
	int i = x->count - 1;
	if (x->leaf)
	{
		while (i >= 0 && inode->inum < x->keys[i]->inum)
		{
			x->keys[i+1] = x->keys[i];
			i--;
		}
		x->keys[i+1] = inode;
		x->count++;
	}
	else
	{
		while (i >= 0 && inode->inum < x->keys[i]->inum)
		{
			i--;
		}
		i++;
		if (x->ptrs[i]->count == 2*t-1)
		{
			bTreeSplitChild(x, i+1, x->ptrs[i], t);
			if (inode->inum > x->keys[i]->inum)
			{
				i++;
			}
		}
		bTreeInsertNonFull(x->ptrs[i], inode, t);
	}
}

void bTreeSplitChild(node *x, int i, node *y, int t)
{
	node *z = createEmptyNode(t);
	z->leaf = y->leaf;
	z->count = t-1;
	int j;
	for (j = 0; j < t-1; j++)
	{
		z->keys[j] = y->keys[j+t];
	}
	if (!y->leaf)
	{
		for (j = 0; j < t; j++)
		{
			z->ptrs[j] = y->ptrs[t+j];
		}
	}
	y->count = t-1;
	for (j = x->count+1; j > i; j--)
	{
		x->ptrs[j] = x->ptrs[j-1];
	}
	x->ptrs[i] = z;
	for (j = x->count-1; j >= i-1; j--)
	{
		x->keys[j+1] = x->keys[j];
	}
	x->keys[i-1] = y->keys[t-1];
	x->count++;
}

void deleteNodes(node *n)
{
	int i;
	for (i = 0; i < n->count+1; i++)
	{
		if (n->ptrs[i] != NULL)
		{
			deleteNodes(n->ptrs[i]);
		}
	}
	for (i = 0; i < n->count; i++)
	{
		//free(n->keys[i]->data);
		//n->keys[i]->data = NULL;
		free(n->keys[i]);
		n->keys[i] = NULL;
	}
	free(n->keys);
	n->keys = NULL;
	free(n->ptrs);
	n->ptrs = NULL;
	free(n);
	n = NULL;
}

void deleteBTree(btree *T)
{
	deleteNodes(T->root);
	free(T);
}
