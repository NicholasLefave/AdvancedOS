#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "btree.h"

void menu(btree *bt);
void getInput(char *s);
void strip(char *s);

int main(void)
{
	btree *bt = createBTree(2);
	menu(bt);
	deleteBTree(bt);
	return 0;
}

void menu(btree *bt)
{
	char s[10];
	memset(s, 0, 10);
	int count = 0;
	while (strcmp(s, "6"))
	{
		memset(s, 0, 10);
		printf("\n-------------Menu--------------\n");
		printf("1. Add directory\n");
		printf("2. Add file\n");
		printf("3. Print file system\n");
		printf("4. Search for file\n");
		printf("5. Change present working directory\n");
		printf("6. Exit\n");
		printf("-------------------------------\n");
		printf("\nEnter your selection: ");
		getInput(s);

		if (!strcmp(s, "1"))
		{
			printf("\n-------------Add Directory--------------\n");
			printf("\nEnter the full or relative path name of the directory (example: /home/testdir or ./testdir): ");
			char name[9001];
			memset(name, 0, 9001);
			getInput(name);
			bTreeInsert(bt, createInode(bt, name, 0, 1, count++));
		}
		else if (!strcmp(s, "2"))
		{
			printf("\n-------------Add File--------------\n");
			printf("\nEnter the full or relative path name of the file (example: /home/testfile.txt or ./testfile.txt): ");
			char name[9001];
			memset(name, 0, 9001);
			getInput(name);
			printf("\nEnter the size of the file: ");
			char size[20];
			memset(size, 0, 20);
			getInput(size);
			bTreeInsert(bt, createInode(bt, name, atoi(size), 0, count++));
		}
		else if (!strcmp(s, "3"))
		{
			bTreePrint(bt->root, bt->t, 0);
		}
		else if (!strcmp(s, "4"))
		{
			printf("\n-------------Search--------------\n");
			printf("\nEnter the full or relative path name of the file or directory to search for (example: /home/testfile.txt or ./testfile.txt): ");
			char name[9001];
			memset(name, 0, 9001);
			getInput(name);
			char *ptr = name;
			convertToAbsolute(&ptr, bt->pwd);
			inode *in = tableSearchName(bt->table, name);
			if (in == NULL)
			{
				printf("\nError 404: File or directory was not found!");
			}
			else
			{
				printf("\nSuccess! File or directory was found.");
				printf("\nName: %s", in->name);
				printf("\nID: %i", in->id);
				if (in->dir)
				{
					printf("\nType: Directory");
					printf("\nNumber of contained objects: %i", in->filecount);
					printf("\nID's of objects: ");
					int i;
					for (i = 0; i < in->filecount; i++)
					{
						printf("%i, ", in->files[i]);
					}
				}
				else
				{
					printf("\nType: File");
					printf("\nSize: %i", in->size);
				}
				printf("\nUser: %i	|	Group: %i	| Device: %i\n\n", in->user, in->group, in->device);
			}
		}
		else if (!strcmp(s, "5"))
		{
			printf("\n-------------Change Present Working Directory--------------\n");
			printf("\nThis is your current present working directory: %s", bt->pwd);
			printf("\nEnter the full or relative path name of the new present working directory (example: /home/testdir or ./testdir): ");
			char name[9001];
			memset(name, 0, 9001);
			getInput(name);
			char *ptr = name;
			convertToAbsolute(&ptr, bt->pwd);
			inode *in = tableSearchName(bt->table, name);
			if (in == NULL && strcmp(name, "/"))
			{
				printf("\nError 404: Directory was not found! Your present working directory was not changed.");
			}
			else
			{
				free(bt->pwd);
				bt->pwd = (char *)malloc((strlen(name)+1) * sizeof(char));
				strcpy(bt->pwd, name);
			}
		}
	}
}

void getInput(char *s)
{
	char c=getchar();
	int i = 0;
	while (c != '\n' && c != EOF)
	{
		s[i++] = c;
		c=getchar();
	}

	strip(s);
}

void strip(char *s)
{
	int len;
  	len = strlen(s);
  	if (len >= 2)
  	{
  		if(s[len - 2] == '\r')
    			s[len -2] = '\0';

  		else if(s[len -1] == '\n')
    			s[len -1] = '\0';
	}
}// end strip

