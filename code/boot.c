#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "shell.h"
#include "system.h"
#include "filemanager.h"
int main( void )
{
	char c;
	char *shm, *s;
	int pid[2];
	int status;

	shm = createSharedMemory();
	setSharedMemoryTo(NOBODY);
//	char data = getSharedMemoryTo();
//	printf("Getting in boot: %s\n", data):

	//TODO: Start CPU
	pid[0] = fork();
	if(pid[0] == 0)
	{
		cpu();
		exit(0);
	}

	//TODO: Start SHELL
	pid[1] = fork();
	if( pid[1] == 0)
	{
		shell();
		exit(0);
	}

	//Start FILEMANAGER
	pid[2] = fork();
	if(pid[2] == 0)
	{
		fileManagerInit();
		exit(0);
	}	

	waitpid(pid[0], &status, 0);
	waitpid(pid[1], &status, 0);
	waitpid(pid[2], &status, 0);
	return 0;
}
