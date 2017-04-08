#include "disk.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct linkedListNode
{
	long start;
	long size;
	struct linkedListNode* next;
}linkedListNode;

void deleteNext(linkedListNode* node);
int sizeOfList();
void deleteHead();

linkedListNode* freeList;

void diskFormat()
{
	if(freeList != NULL) freeTheList();
	freeList = malloc(sizeof(linkedListNode));
	freeList->start = 0;
	freeList->size = DISK_SIZE;
	freeList->next = NULL;
}

char* getSpace(long size)
{
	linkedListNode *curr = freeList,
				   *prev = NULL;
	int temp;
	while(curr != NULL && curr->size < size)
	{
		prev = curr;
		curr = curr->next;
	}
	if(curr == NULL)
		return NULL;
	else
	{
		temp = curr->start;
		curr->size -= size;
		if(curr->size == 0)
			if(prev == NULL)
				deleteHead();
			else
				deleteNext(prev);
		else
			curr->start += size;
		return &disk[temp];
	}
}

char* extendSpace(char* start, long currSize, long newSize)
{
	int placement = start - disk;
	linkedListNode *curr,
				   *prev;
	prev = NULL;
	curr = freeList;

	if (freeList == NULL)
		return NULL;

	while(curr != NULL && curr->start < placement)
	{
		prev = curr;
		curr = curr->next;
	}

	if (curr != NULL && curr->start == (placement + currSize) && (curr->start + curr->size) >= (placement + newSize))
	{
		curr->size -= (newSize - currSize);
		if (curr->size == 0)
			if (prev == NULL)
				deleteHead();
			else
				deleteNext(prev);
		else
			curr->start += (newSize - currSize);
		return start;
	}
	else
	{
		char* newStart;
		char* dataTemp = (char *)calloc(currSize+1, sizeof(char));
		strncpy(dataTemp, start, currSize);
		deleteSpace(start, currSize);
		newStart = getSpace(newSize);
		strncpy(newStart, dataTemp, currSize);
		free(dataTemp);
		return newStart;
	}

	return NULL;
}

int deleteSpace(char* start, long size)
{
	int placement = start - disk;
	assert(placement >= 0 && placement < DISK_SIZE);
	linkedListNode *curr, 
				   *prev;
	prev = NULL;
	curr = freeList;
	
	if(freeList == NULL) 
	{
		freeList = malloc(sizeof(linkedListNode));
		freeList->next = NULL;
		freeList->start = placement;
		freeList->size = size;
		return 0;
	}

	while(curr != NULL && curr->start < (placement + size))
	{
		prev = curr;
		curr = curr->next;
	}
	//shift to the right
	if(prev != NULL && (prev->start + prev->size) == placement)
	{
		prev->size += size;
		if(curr != NULL && (prev->start + prev->size) == curr->start)
		{
			prev->size += curr->size;
			deleteNext(prev);
		}
	}
	//shift to the left
	else if(curr != NULL && curr->start == (placement + size))
	{
		curr->start -= size;
	}
	//create new node
	else
	{
		linkedListNode *temp = malloc(sizeof(linkedListNode));
		temp->start = placement;
		temp->size = size;

		if(curr == NULL)
		{
			prev->next = temp;
			temp->next = NULL;
		}
		else if(prev == NULL)
		{
			temp->next = curr;
			freeList = temp;
		}
		else
		{
			temp->next = curr;
			prev->next = temp;
		}
		
	}
	return 0;
}

freeTheList()
{
	linkedListNode *curr, *prev = freeList;
	for(curr = prev->next; curr != NULL; curr = curr->next)
	{
		free(prev);
		prev = curr;
	}
	free(prev);
}

void deleteNext(linkedListNode* node)
{
	if(node == NULL || node->next == NULL) return;
	
	linkedListNode *temp = node->next;
	node->next = node->next->next;
	free(temp);
}

void deleteHead()
{
	linkedListNode *temp = freeList;
	freeList = freeList->next;
	free(temp);
}

int sizeOfList()
{
	linkedListNode *curr = freeList;
	int i;
	for(i = 0; curr != NULL; curr = curr->next)
	{
		++i;
	}
	return i;
}

int writeOutFreeList()
{
	//permanent storage stuff
	
	freeTheList();
}

void deleteDisk()
{
	freeTheList();
}
