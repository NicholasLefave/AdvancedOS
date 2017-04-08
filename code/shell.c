#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "system.h"
#include <string.h>
//#include <unistd.h>
#include "shellcommands.h"
#include "filemanager.h"
//#include <conio.h>


typedef struct commandHistory
{
	char *command;
	struct commandHistory * next;
}commandHistory;

void freeHistory(commandHistory *head);

//Strips off the new line character of a string
void strip(char *s)
{
	int len; 
	len = strlen(s);
	if(s[len - 2] == '\r')
	{
		s[len - 2] = '\0';
	}
	else if(s[len - 1] == '\n')
	{
		s[len -1] = '\0';
	}
}//end strip


int shell()
{
	//for ipc memory
	char *shm;
	
	//Path -- keeps track of the current working directory

	sprintf(path, "/");
	
	//set up IPC
	shm = getSharedMemory();
	//Loop keeps going (reading the terminal input) until the user types in 'exit'
	char** prams; 

	commandHistory *head; // dummy head node for our command history list. 
	head = malloc(sizeof(commandHistory));
	head->next = NULL;
	head->command = NULL;
	
	int loop = 1;
	while(loop == 1)
	{
		//read input from termina
		int count = 1;
		char input[256];
		printf("terminal: ");
		fgets(input, 256, stdin);
	        strip(input);
		while(!requestSharedMemory(SHELL));
		commandHistory *node;
		node = malloc(sizeof(commandHistory));
		node->command =malloc(sizeof(char) * strlen(input) + 1);
		strcpy(node->command, input );
		node->next = head->next;
		head->next = node; //point head to our new node
	
		if(strcmp("exit", input) == 0)
		{
			tellCpuToExit();
			tellFileManagerToExit();
			loop = 0;
			freeHistory(head);
			continue;
		}
		else //parse input
		{
			char *singlePram; 
			count = countprams(input, 256);
			singlePram = strtok(input, " "); 
			prams = calloc(count, sizeof(char*) );	
			int i = 0;
			while(singlePram != NULL)
			{
				prams[i] = calloc(strlen(singlePram)+1, sizeof(char));
				strcpy(prams[i], singlePram);
			//	printf("%s\n", prams[i]); //Right now we are just printing out our prams
				// later we will want to pass these
				singlePram = strtok(NULL, " \n");
				i++;
			}
			
		}
		if(strcmp("echo", prams[0]) == 0)
		{
			echo(count, prams);
		}
		else if(strcmp("cd", prams[0]) == 0)
		{
			if(count == 2)
			{
				char pathSave[256];
				sprintf(pathSave, "%s", path);
				if(!strcmp(prams[1], "/"))
				{
					//you want to go to root
					sprintf(path, "/");
				}
				else if(prams[1][0] == '.' && prams[1][1] == '.')
				{
					int depth = countFolderDepth(path, 256);
					moveToDepth(path, depth - 1);	
				}
				else if(prams[1][0] == '.' && prams[1][1] == '/')
				{
					int size = strlen(prams[1]);
					char* temp = calloc(size - 2, sizeof(char));
					strncpy(temp, prams[1] + 2, size - 2);
					strcat(path, temp);
					strcat(path, "/");
					free(temp);
				}
				else if(prams[1][0] != '/')
				{
					// you want to append your current path
					strcat(path, prams[1]);
					strcat(path, "/");
				}
				else
				{
					//you put in full path
					sprintf(path, "%s/", prams[1]);
				}
				
				if(strcmp(prams[1], "/") && strcmp(prams[1], "..")) //if not wanting to go to root. 
				{
				
					char *data;
					data = getSharedMemoryData();
					char path_minus_slash[256];
					sprintf(path_minus_slash, "%s", path);
					int length = strlen(path_minus_slash);
					path_minus_slash[length - 1] = '\0';
					sprintf(data, "%s", path_minus_slash);
					int fd;
					((int*)data+(strlen(data)+1))[0] = FREAD;
					((int*)data+(strlen(data)+1))[1] = 0;	
					setSharedMemoryCommand(COPEN);	
					setSharedMemoryFrom(SHELL);
					setSharedMemoryTo(FILEMANAGER);
					while(!canIAccessSharedMemory(SHELL))
					{
						//wait untill I hear back from the filemanager
					}
					data = getSharedMemoryData();
					fd = ((int*)data)[0];
					if(fd == -1)
					{
						printf("Unable to cd: Not a valid path\n");
						sprintf(path, "%s", pathSave);
					}
				}	

			}
		}
		
		else if(strcmp("pwd", prams[0]) == 0)
		{
			printf("%s\n", path);
		}
		else if(strcmp("history", prams[0]) == 0)
		{
			commandHistory *i = head->next;
			for(; i != NULL; i = i->next)
			{
				printf("%s\n", i->command);
			}
		}
		
		else if(strcmp("ls", prams[0]) == 0)
		{
			ls(count, prams);
		}
		else if(strcmp("prog1.exe", prams[0]) == 0)
		{
			program(count, prams);
		}
		else if(strcmp("prog2.exe", prams[0]) == 0)
		{
			program(count, prams);
		}
		else if(strcmp("prog3.exe", prams[0]) == 0)
		{
			program(count, prams);
		}
		else if(strcmp("prog4.exe", prams[0]) == 0)
		{
			program(count, prams);
		}
		else if(strcmp("prog5.exe", prams[0]) == 0)
		{
			program(count, prams);
		}
		else if(strcmp("prog6.exe", prams[0]) == 0)
		{
			program(count, prams);
		}
		else if(strcmp("problem6.exe", prams[0]) == 0)
		{
			program(count, prams);
		}
		else if(strcmp("demo", prams[0]) == 0)
		{
			testCommand();
		}
		else if(strcmp("clear", prams[0]) == 0)
		{
			int c;
			for(c = 0; c < 100; c++)
			{
				printf("\n");
			}
		}

		else if(strcmp("touch", prams[0]) == 0)
		{
			char* temp;
			temp = malloc(sizeof(char) * (strlen(path) + strlen(prams[1])));
			strcpy(temp, path);
			strcat(temp, prams[1]);
			prams[1] = malloc(sizeof(char) * (strlen(path) + strlen(prams[1])));
			strcpy(prams[1], temp);
			touch(count, prams);
		}
		else if(strcmp("mkdir", prams[0]) == 0)
		{
			char* temp;
			temp = malloc(sizeof(char) * (strlen(path) + strlen(prams[1])));
			strcpy(temp, path);
			strcat(temp, prams[1]);
			prams[1] = malloc(sizeof(char) * (strlen(path) + strlen(prams[1])));
			strcpy(prams[1], temp);
			mkdir(count, prams);
		}
		else if(strcmp("cp", prams[0]) == 0)
		{
			cp(count, prams);
		}
		else if(strcmp("write", prams[0]) == 0)
		{
			write(count, prams);
		}
		else if(strcmp("mv", prams[0]) == 0)
		{
			cp(count, prams);
			removeFile(count, prams);
		}
		else if(strcmp("cat", prams[0]) == 0)
		{
			cat(count, prams);
		}

		else if(strcmp("rm", prams[0]) == 0)
                {
                        removeFile(count, prams);
               	}
		else if(strcmp("rmdir", prams[0]) == 0)
                {
                	rmdir(count, prams);
		}
		else if(strcmp("help", prams[0]) == 0)
		{
			help();
		}
		else
		{
			printf("Not A Valid Comand\n");
		}
		setSharedMemoryTo(NOBODY);
		int i;
		for(i = 0; i < count; ++i)
		{
			free(prams[i]);
		}
		free(prams);

	}		
	
	return 0;
}

int countprams(char* input, int size)
{
	char* temp;
	temp = malloc(sizeof(char) * size);
	strcpy(temp, input);
	char* token;
	token = strtok(temp, " \n");
	int count = 0; 
	while(token != NULL)
	{
		count++;
		token = strtok(NULL, " \n");
	}
	free(temp);
	return count;
}

int countFolderDepth(char* input, int size)
{
	char* temp;
	temp = calloc(size, sizeof(char));
	strcpy(temp, input);
	char* token;
	token = strtok(temp, "/");
	int count = 0; 
	while(token != NULL)
	{
		count++;
		token = strtok(NULL, "/");
	}
	return count;
}

void moveToDepth(char* input, int depth) //input should be the path //depth is how far you wanna go back
{
	char* temp;
	temp = calloc(256, sizeof(char));//path is 256
	strcpy(temp, input);
	char* token;
	token = strtok(temp, "/");
	int count = 0; 
	strcpy(path, "/");
	while(token != NULL && count < depth)
	{
		strcat(path, token);
		strcat(path, "/");
		token = strtok(NULL, "/");
		count++;
	}

}

void freeHistory(commandHistory *head)
{
	commandHistory *curr = head,*prev = NULL;
	for(curr = head; curr != NULL; )
	{
		prev = curr;
		curr = curr->next;
		if(prev->command) free(prev->command);
		free(prev);
	}
}
