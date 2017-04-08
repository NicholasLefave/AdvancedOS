#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "filemanager.h"
#include "btree.h"
#include "disk.h"
#include "system.h"

#define FILE_TABLE_SIZE 1024

typedef struct fileDes {
	inode* iNode;
	int position;
}fileDes;

int getNextFileDescriptor();

fileDes* fileTable[FILE_TABLE_SIZE];
int nextFileDescriptor;
int currentFile;
int running = 0;
btree *systemBTree;

int fileManagerInit()
{	
	memset(&fileTable, 0, FILE_TABLE_SIZE);
	diskFormat();    
	systemBTree = createBTree(2);

	inode* root = createInode(systemBTree);
	root->size = 9;
	root->data = getSpace(9);
	root->filemode = 01755;

	int fd = Open("/", FWRITE, 0);
	char data[9] = ".#0\n..#0\n";
	Write(fd, &data, 9);
	Close(fd);

	running = 1;
	checkSharedMemory();

	return 0;
}

void checkSharedMemory()
{
	char from;
	char command;
	char *data = NULL;
	char *buffer = NULL;
	int streamedData = 0;

	while(running)
	{
		if (canIAccessSharedMemory(FILEMANAGER))
		{
			data = getSharedMemoryData();
			from = getSharedMemoryFrom();
			command = getSharedMemoryCommand();

			if (command == CMOREDATA)
			{
				char *tempBuffer = NULL;
				while(command == CMOREDATA)
				{
					if (buffer != NULL)
					{
						tempBuffer = (char *)calloc(strlen(buffer)+1, sizeof(char));
						strncpy(tempBuffer, buffer, strlen(buffer));
						free(buffer);
						buffer = NULL;
					}
					if (tempBuffer != NULL)
					{
						buffer = (char *)calloc(getSharedDataSize() + strlen(tempBuffer) + 1, sizeof(char));
						strncpy(buffer, tempBuffer, strlen(tempBuffer));
						strncat(buffer, data, getSharedDataSize());
						free(tempBuffer);
						tempBuffer = NULL;
					}
					else
					{
						buffer = (char *)calloc(getSharedDataSize() + 1, sizeof(char));
						strncpy(buffer, data, getSharedDataSize());
					}
					setSharedMemoryFrom(FILEMANAGER);
					setSharedMemoryCommand(CCONTINUE);
					setSharedMemoryTo(from);
					while(!canIAccessSharedMemory(FILEMANAGER));
					command = getSharedMemoryCommand();
				}
				tempBuffer = (char *)calloc(strlen(buffer) + 1, sizeof(char));
				strncpy(tempBuffer, buffer, strlen(buffer));
				free(buffer);
				buffer = (char *)calloc(getSharedDataSize() + strlen(tempBuffer) + 1, sizeof(char));
				strncpy(buffer, tempBuffer, strlen(tempBuffer));
				strncat(buffer, data, getSharedDataSize());
			}

			if (buffer != NULL)
				streamedData = 1;
			else
				buffer = data;
			
			setSharedMemoryFrom(FILEMANAGER);
			switch(command)
			{
				case CTESTCOMMAND:
					setSharedMemoryCommand(CDATA);
					sprintf(data, "Hello World!\0");
					break;
				case CDEMO:
					demoPopulate();
					break;
				case COPEN:
					setSharedMemoryCommand(CDATA);
		 			((int*)buffer)[0] = Open(buffer, ((int*)buffer+(strlen(buffer)+1))[0], ((int*)buffer+(strlen(buffer)+1))[1]);
					break;
				case CCLOSE:
					setSharedMemoryCommand(CDATA);
					((int*)buffer)[0] = Close(((int*)buffer)[0]);
					break;
				case CREAD:
					setSharedMemoryCommand(CDATA);
					Read(((int*)buffer)[0], buffer, ((int*)buffer)[1]);
					break;
				case CWRITE:
					setSharedMemoryCommand(CDATA);
					((int*)buffer)[0] = Write(((int*)buffer)[0], &buffer[sizeof(int)*2], ((int*)buffer)[1]);
					break;
				case CSEEK:
					setSharedMemoryCommand(CDATA);
					((int*)buffer)[0] = seek(((int*)buffer)[0], ((int*)buffer)[1], ((int*)buffer)[2]);
					break;
				case CDELETE:
					setSharedMemoryCommand(CDATA);
					((int*)buffer)[0] = delete(buffer);
					break;
				case CGETMETADATA:
					getMetaData(buffer, from);
					break;
				case CGETSIZE:
					((int*)buffer)[0] = getSize(((int*)buffer)[0]);
					break;
				case CGETPERMISSIONS:
					((int*)buffer)[0] = getFilePermissions(buffer);
					break;
				case CEXIT:
					fileManagerShutdown();
					break;
				default:
					setSharedMemoryCommand(CCOMMANDFAILED);
					break;
			}
			setSharedMemoryTo(from);

			if (streamedData)
			{
				free(buffer);
				streamedData = 0;
			}
			buffer = NULL;
		}
	}
}

int fileManagerShutdown()
{
	deleteBTree(systemBTree);
	deleteDisk();
	running = 0;
	return 0;
}

int Open(char *path, int flags, int mode)
{
	char *pathCopy;
	if (path[0] != '/') 
	{
		pathCopy = (char *)calloc(strlen(path)+2, sizeof(char));
		pathCopy[0] = '/';
		strcpy(pathCopy+1, path);
	}
	else
	{
		pathCopy = (char *)calloc(strlen(path)+1, sizeof(char));
		strcpy(pathCopy, path);
	}
	int fd = getNextFileDescriptor();
	
	fileDes *newFileDes = malloc(sizeof(fileDes));
	fileTable[fd] = newFileDes;	

	if(flags & FCREATE)
	{
		newFileDes->position = 0;
		newFileDes->iNode = createInode(systemBTree);
		newFileDes->iNode->data = getSpace(0);
		newFileDes->iNode->size = 0;	
		newFileDes->iNode->filemode = mode;
		newFileDes->iNode->created = time(0);
		newFileDes->iNode->modified = time(0);

		int dirfd;
		int stringLen;
		char *lastSlash = strrchr(pathCopy, '/');
		if (lastSlash-pathCopy != 0)
		{
			stringLen = lastSlash-pathCopy;
			char *parentDirPath = (char *)calloc(stringLen+1, sizeof(char));
			strncpy(parentDirPath, path, stringLen);
			dirfd = Open(parentDirPath, FWRITE | APPEND, 0);
			free(parentDirPath);
		}
		else
			dirfd = Open("/\0", FWRITE | APPEND, 0);
		char integer[11];
		sprintf(integer, "%d\0", newFileDes->iNode->inum);
		stringLen = strlen(lastSlash+1) + strlen(integer) + 3;
		char fileName[stringLen];
		snprintf(fileName, stringLen+1, "%s#%s\n", lastSlash+1, integer);
		fileTable[dirfd]->position = fileTable[dirfd]->iNode->size;
		Write(dirfd, fileName, stringLen-1);

		if ((newFileDes->iNode->filemode & 01000))
		{
			char parentInteger[11];
			sprintf(parentInteger, "%d\0", fileTable[dirfd]->iNode->inum);
			stringLen = strlen(parentInteger) + strlen(integer) + 8;
			char data[stringLen];
			snprintf(data, stringLen, ".#%s\n..#%s\n", integer, parentInteger);
			newFileDes->position = newFileDes->iNode->size;
			Write(fd, data, stringLen-1);
		}

		Close(dirfd);
	}
	else
	{
		newFileDes->iNode = bTreeSearchPath(systemBTree, pathCopy);
		if (newFileDes->iNode == NULL)
			return -1;
		newFileDes->position = (flags & APPEND) ? newFileDes->iNode->size : 0;
		//TODO: Truncate needs to be fixed
	}
	free(pathCopy);

	return fd;
}

int Close(int fd)
{
	if(fileTable[fd]) free(fileTable[fd]);
	fileTable[fd] = NULL;
	nextFileDescriptor = nextFileDescriptor < fd ? nextFileDescriptor : fd;
	return 0;
}

int Read(int fd, void *buffer, unsigned int size)
{
	fileDes* tempNode = fileTable[fd];
	if(tempNode == NULL) return -1;
	
	int i,stop = size;
	if((tempNode->position + size) > tempNode->iNode->size)
		stop = tempNode->iNode->size - tempNode->position;
	
	for(i = 0; i < stop; ++i)
		*((char*)buffer + i) = *(tempNode->iNode->data + tempNode->position++);
	
	return i;
}

int Write(int fd, void *buffer, unsigned int count)
{
	fileDes* tempNode = fileTable[fd];
	if(tempNode == NULL) return -1;
	
	int i,stop = count;
	if (tempNode->position + count > tempNode->iNode->size)
	{
		tempNode->iNode->data = extendSpace(tempNode->iNode->data, tempNode->iNode->size, (tempNode->position + count));
		tempNode->iNode->size = tempNode->position + count;
	}
	for(i = 0; i < stop; ++i)
		tempNode->iNode->data[tempNode->position++] = *((char*)buffer + i);

	tempNode->iNode->modified = time(0);
}

int seek(int fd, int offset, int whence)
{
	fileDes* tempNode = fileTable[fd];
	if (whence & SEEKSET)
		tempNode->position = 0;
	if (whence & SEEKEND)
		tempNode->position = tempNode->iNode->size;

	tempNode->position += offset;
	if(tempNode->position > tempNode->iNode->size)
	{
		tempNode->position = tempNode->iNode->size;
		return -1;
	}
	else if(tempNode->position < 0)
	{
		tempNode->position = 0;
		return -1;	
	}
	return tempNode->position;
}

int getNextFileDescriptor()
{
	int i; 
	for(i = 0; i < FILE_TABLE_SIZE; i++)
	{
		if(fileTable[i] == NULL)
		{
			nextFileDescriptor = i;
			break;
		}
	}
	return i;
}

//ToDo: Delete inode of file from btree
int delete(char *path)
{
	char* lastSlash = strrchr(path, '/');
	if (!strcmp(path, "/"))
		return -1;

	char* parentDirPath = (char*)calloc((lastSlash-path == 0 ? 1 : lastSlash-path)+1, sizeof(char));
	strncpy(parentDirPath, path, lastSlash-path == 0 ? 1 : lastSlash-path);

	inode* node = bTreeSearchPath(systemBTree, path);
	deleteSpace(node->data, node->size);

	node = bTreeSearchPath(systemBTree, parentDirPath);
	int currSize = node->size;
	char* data = (char*)calloc(currSize+1, sizeof(char));
	strncpy(data, node->data, currSize);

	char* ptr = strstr(data, lastSlash+1);
	int offset = (int)strcspn(ptr, "\n")+1;
	strncpy(ptr, ptr+offset, currSize-(ptr-data)-offset);
	deleteSpace(node->data, currSize);
	node->data = getSpace(currSize-offset);
	node->size = currSize-offset;
	strncpy(node->data, data, currSize-offset);
	node->modified = time(0);

	free(parentDirPath);

	return 0;
}

int getMetaData(char *path, char from)
{
	char* data = getSharedMemoryData();
	char* lastSlash = strrchr(path, '/');
	if (strcmp(path, "/"))
		lastSlash = lastSlash + 1;
	inode* node = bTreeSearchPath(systemBTree, path);
	char filemode[11];
	char nodesize[11];
	sprintf(filemode, "%04o", node->filemode);
	sprintf(nodesize, "%d", (int)node->size);
	int size = strlen(filemode) + strlen(nodesize) + strlen(asctime(localtime(&node->modified))) + strlen(lastSlash) + 3;
	char* buffer = (char*)calloc(size, sizeof(char));
	sprintf(buffer, "%04o\n%d\n%s%s", node->filemode, (int)node->size, asctime(localtime(&node->modified)), lastSlash);
	
	int x, count = 0;
	while(size > getSharedDataSize())
	{
		setSharedMemoryFrom(FILEMANAGER);
		setSharedMemoryCommand(CMOREDATA);
		for (x = 0; x < getSharedDataSize(); x++)
			memcpy(data, buffer + count++, sizeof(char));
		setSharedMemoryTo(from);
		size = size - getSharedDataSize();
		
		while(!canIAccessSharedMemory(FILEMANAGER));
	}

	setSharedMemoryFrom(FILEMANAGER);
	setSharedMemoryCommand(CDATA);
	strcpy(data, buffer + count);

	free(buffer);
	
	return 0;
}

int getSize(int fd)
{
	return (int)fileTable[fd]->iNode->size;
}

int getFilePermissions(char *path)
{
	inode* node = bTreeSearchPath(systemBTree, path);
	return node->filemode;
}

void demoPopulate()
{
	int fd = Open("/etc", FCREATE, 01755);
	Close(fd);
	fd = Open("/usr", FCREATE, 01755);
	Close(fd);
	fd = Open("/home", FCREATE, 01755);
	Close(fd);
	fd = Open("/home/tom", FCREATE, 01755);
	Close(fd);
	fd = Open("/home/nick", FCREATE, 01755);
	Close(fd);
	fd = Open("/home/conrad", FCREATE, 01755);
	Close(fd);
	fd = Open("/home/steven", FCREATE, 01755);
	Close(fd);
	fd = Open("/home/tom/Documents", FCREATE, 01755);
	Close(fd);
	fd = Open("/home/nick/Documents", FCREATE, 01755);
	Close(fd);
	fd = Open("/home/conrad/Documents", FCREATE, 01755);
	Close(fd);
	fd = Open("/home/steven/Documents", FCREATE, 01755);
	Close(fd);
	fd = Open("/home/tom/Documents/data.txt", FCREATE | FWRITE, 00777);
	char tomData[24] = "Hello, I am Tom's file!\0";
	Write(fd, tomData, 24);
	Close(fd);
	fd = Open("/home/nick/Documents/data.txt", FCREATE | FWRITE, 00777);
	char nickData[25] = "Hello, I am Nick's file!\0";
	Write(fd, nickData, 25);
	Close(fd);
	fd = Open("/home/conrad/Documents/data.txt", FCREATE | FWRITE, 00777);
	char conradData[27] = "Hello, I am Conrad's file!\0";
	Write(fd, conradData, 27);
	Close(fd);
	fd = Open("/home/steven/Documents/data.txt", FCREATE | FWRITE, 00777);
	char stevenData[27] = "Hello, I am Steven's file!\0";
	Write(fd, stevenData, 27);
	Close(fd);
	fd = Open("/abc", FCREATE | FWRITE, 00777);
	char abcData[12] = "number: 1234";
	Write(fd, abcData, 12);
	Close(fd);
	fd = Open("/xyz", FCREATE | FWRITE, 00777);
	char xyzData[12] = "number: 4321";
	Write(fd, xyzData, 12);
	Close(fd);
	fd = Open("/ijklmn", FCREATE | FWRITE, 00777);
	Close(fd);
}
