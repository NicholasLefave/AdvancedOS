#include "filemanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"

int main(int argc, char ** argv)
{
	int fd = -1;
	int pid;	
	char *tester;
	int size;

	createSharedMemory();
	
	pid = fork();
	if (pid == 0)
	{
		fileManagerInit();
		exit(0);
	}

	tester = getSharedMemoryData();

	//-------Run Test Command----------

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CTESTCOMMAND);
	setSharedMemoryTo(FILEMANAGER);

	while (!canIAccessSharedMemory(SHELL));

	printf("\n%s\n", tester);

	//---Open and create file "/file"---

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(COPEN);
	sprintf(tester, "/file\0");
	((int*)tester+(strlen(tester)+1))[0] = CREATE;
	((int*)tester+(strlen(tester)+1))[1] = 00755;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	fd = ((int*)tester)[0];

	//---------Close "/file"-----------

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CCLOSE);
	((int*)tester)[0] = fd;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	//---Open and create file "/anotherFile"---
	
	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(COPEN);
	sprintf(tester, "/anotherFile\0");
	((int*)tester+(strlen(tester)+1))[0] = CREATE;
	((int*)tester+(strlen(tester)+1))[1] = 00755;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	fd = ((int*)tester)[0];

	//---------Close "/anotherFile"-----------
	
	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CCLOSE);
	((int*)tester)[0] = fd;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	//---Open and create dir "/subDir"---

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(COPEN);
	sprintf(tester, "/subDir\0");
	((int*)tester+(strlen(tester)+1))[0] = CREATE;
	((int*)tester+(strlen(tester)+1))[1] = 01755;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	fd = ((int*)tester)[0];

	//---------Close "/subDir"-----------
	
	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CCLOSE);
	((int*)tester)[0] = fd;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	//---Open and create file "/subDir/subFile"---
	
	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(COPEN);
	sprintf(tester, "/subDir/subFile\0");
	((int*)tester+(strlen(tester)+1))[0] = CREATE;
	((int*)tester+(strlen(tester)+1))[1] = 00755;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	fd = ((int*)tester)[0];

	//---------Close "/subDir/subFile"-----------
	
	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CCLOSE);
	((int*)tester)[0] = fd;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	//--------Open, seek, and read "/"--------

	tester = getSharedMemoryData();
	
	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(COPEN);
	sprintf(tester, "/\0");
	((int*)tester+(strlen(tester)+1))[0] = READ;
	((int*)tester+(strlen(tester)+1))[1] = 0;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();
	fd = ((int*)tester)[0];

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CSEEK);
	((int*)tester)[0] = fd;
	((int*)tester)[1] = 0;
	((int*)tester)[2] = SEEKEND;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();
	size = ((int*)tester)[0];

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CSEEK);
	((int*)tester)[0] = fd;
	((int*)tester)[1] = 0;
	((int*)tester)[2] = SEEKSET;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();
	size = size - ((int*)tester)[0];

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CREAD);
	((int*)tester)[0] = fd;
	((int*)tester)[1] = size;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();

	printf("\n--------/--------\n");
	int x;
	for (x = 0; x < size; x++)
		printf("%c", tester[x]);

	//-----------Close "/"-------------

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CCLOSE);
	((int*)tester)[0] = fd;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	//--------Open, seek, and read "/subDir"--------

	tester = getSharedMemoryData();
	
	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(COPEN);
	sprintf(tester, "/subDir\0");
	((int*)tester+(strlen(tester)+1))[0] = READ;
	((int*)tester+(strlen(tester)+1))[1] = 0;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();
	fd = ((int*)tester)[0];

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CSEEK);
	((int*)tester)[0] = fd;
	((int*)tester)[1] = 0;
	((int*)tester)[2] = SEEKEND;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();
	size = ((int*)tester)[0];

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CSEEK);
	((int*)tester)[0] = fd;
	((int*)tester)[1] = 0;
	((int*)tester)[2] = SEEKSET;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CREAD);
	((int*)tester)[0] = fd;
	((int*)tester)[1] = size;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	printf("\n--------/subDir------------\n");
	for (x = 0; x < size; x++)
		printf("%c", tester[x]);

	//-----------Close "/"-------------

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CCLOSE);
	((int*)tester)[0] = fd;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	//--------Open, write, seek, and read "/subDir/subFile"--------

	tester = getSharedMemoryData();
	
	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(COPEN);
	sprintf(tester, "/subDir/subFile\0");
	((int*)tester+(strlen(tester)+1))[0] = READ | WRITE;
	((int*)tester+(strlen(tester)+1))[1] = 0;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL))

	tester = getSharedMemoryData();
	fd = ((int*)tester)[0];

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CWRITE);
	((int*)tester)[0] = fd;
	((int*)tester)[1] = 19;
	sprintf(tester + (sizeof(int)*2), "Hello, I am a file!");
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CGETSIZE);
	((int*)tester)[0] = fd;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();
	size = ((int*)tester)[0];

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CSEEK);
	((int*)tester)[0] = fd;
	((int*)tester)[1] = 0;
	((int*)tester)[2] = SEEKSET;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CREAD);
	((int*)tester)[0] = fd;
	((int*)tester)[1] = size;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	tester = getSharedMemoryData();

	printf("\n--------/subDir/subFile------------\n");
	for (x = 0; x < size; x++)
		printf("%c", tester[x]);

	printf("\n");

	//-----------Close "/"-------------

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CCLOSE);
	((int*)tester)[0] = fd;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	//-------get metadata of /subDir/subFile--------

	tester = getSharedMemoryData();

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CGETMETADATA);
	sprintf(tester, "/subDir/subFile\0");
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	printf("\n--------metadata of /subDir/subFile--------\n");
	printf("%s\n", tester);

	//-------Delete /subDir/subFile-------

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CDELETE);
	sprintf(tester, "/subDir/subFile\0");
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));

	//------Shut down file manager-----

	printf("\n\n");

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CEXIT);
	setSharedMemoryTo(FILEMANAGER);

	waitpid(pid, NULL, 0);
	
	return 0;
}
