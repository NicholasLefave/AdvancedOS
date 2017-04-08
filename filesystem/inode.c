/*
 *Steven Kakoczky
 */
#include "myInode.h"
#include <string.h>
#include <stdlib.h>

int NEXT_INUM = 0;

void inodeInit(myInode *node)
{
	node->inum = NEXT_INUM++; /*TODO: abstract for inum reuse*/
	node->uid = getuid();
	node->gid = getgid();
	node->size = 0;
}

int inodeCompare(void *left, void *right) {
	return (left ? ((myInode*)left)->inum : 0) - (right ? ((myInode*)right)->inum : 0);
}

void printInode(void *inode) {
	myInode *node = (myInode*)inode;
	printf("%d: %s", node->inum, node->name);
}

void freeInode(void *inode)
{
	myInode *node = (myInode*)inode;
	free(node->name);
	free(node->data_start);
	free(node);
	/*TODO: reuse inum*/
}

int compName(void *goal, void *curr)
{
	return strcmp(((myInode*)goal)->name, ((myInode*)curr)->name);
}

myInode *findName(node *curr, char *name)
{
	int i;
	myInode *current;
	for(i = 0; i < curr->size; ++i)
	{
		current = (myInode*)curr->data[i];
		if( !strcmp(current->name, name))
			return current;
	}
	if(curr->children[0] == NULL)
		return NULL;
	for(i = 0; i <= curr->size; ++i)
	{
		current = findName(curr->children[i], name);
		if(current != NULL)
			return current;
	}
	return NULL;
}

myInode * myOpen(char *name, int flags)
{
	myInode *node;/* = (myInode*) findAttribute(name, &findName);*/
	if(node == NULL)
	{
		/*if SK_CREAT, make new inode and add to btree
		  else return NULL */
		if(!(flags & SK_CREAT)) return NULL;
		node = (myInode*)malloc(sizeof(myInode));
		insert((void*)node);
	}
	else
	{
		/* TODO: permissions */
	}
	return node;
}/*END Open*/

int myWrite(myInode *file, char *data, int size)
{
	return 0;
}/*END Write*/

int myRead(myInode *file, char *buf, int size)
{
	return 0;
}/*END Read*/
