#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "btree.h"

btree *createBTree(int t)
{
	btree *bt = (btree *)malloc(sizeof(btree));
	bt->t = t;
	bt->root = createEmptyNode(t);
	
	return bt;
}

inode *createInode(int size, int filemode, char *name )
{
	inode *in = (inode *)malloc(sizeof(inode));
	in->size = size;
	in->filemode = filemode;
	in->created = 0;
	in->modified = 0;
	strcpy(in->name, name);
	in->data = NULL;

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
	for (x = 0; x < (2*t); x++)
	{
		n->ptrs[x] = NULL;
	}

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
