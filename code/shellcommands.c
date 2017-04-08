#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "filemanager.h"
#include "shell.h"

#define BLUE "\033[01;34m"
#define WHITE "\x1B[37m"
#define GREEN "\x1B[32m"
#define NORMAL "\x1B[0m"
//char *data = NULL;
//char *buffer = NULL;

void echo(int argc, char** argv)
{
	int i;
	for(i = 1; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");

}

void mkdir(int argc, char** argv)
{
	

	char to = getSharedMemoryTo();
	while(!canIAccessSharedMemory(SHELL))
	{
	}
	//now I have access to shared memory	
	
	//write command	
	setSharedMemoryCommand(COPEN); //ENTER COMMAND HERE

	//1st write to the data section
	char* data = getSharedMemoryData();
	sprintf(data, "%s\0", argv[1]);	
	((int*)data+(strlen(data)+1))[0] = FCREATE;
	((int*)data+(strlen(data)+1))[1] = 01755;
	

	//change the from
	setSharedMemoryFrom(SHELL);

	//change the too (I now don't have access)
	setSharedMemoryTo(FILEMANAGER);
	while(!canIAccessSharedMemory(SHELL))
	{
		//wait untill I hear back from the filemanager
	}
	data = getSharedMemoryData();

	int fd = ((int*)data)[0]; //get the file descriptor back but don't really do anything with it right now


		
}
void removeFile(int argc, char** argv)
{
	char to = getSharedMemoryTo();
        while(!canIAccessSharedMemory(SHELL))
        {
        }
        //now I have access to shared memory    
        
        //write command 
        
        char* data = getSharedMemoryData();
        setSharedMemoryCommand(CDELETE); //ENTER COMMAND HERE


	if(argv[1][0] == '.' && argv[1][1] == '/')//using dot slash
		{
		
			sprintf(data, "%s%s\0", path, argv[1]);
		}		
		else if( argv[1][0] == '/')//using full path
		{
			sprintf(data, "%s\0", argv[1]); //path one, source
		}
		else //using current working directory without dotslash 
		{
				sprintf(data, "%s%s\0", path, argv[1]);
		}
        
        //1st write to the data section
        //char* data = getSharedMemoryData();
        int dataSize = getSharedDataSize();
                

        //change the from
        setSharedMemoryFrom(SHELL);

        //change the too (I now don't have access)
        setSharedMemoryTo(FILEMANAGER);
        while(!canIAccessSharedMemory(SHELL))
        {
                //wait untill I hear back from the filemanager
        }
        data = getSharedMemoryData();
//      printf("%s\n", data);   //if you wanna print out returned data  
        
}
void tellFileManagerToExit()
{

	char to = getSharedMemoryTo();
	while(!canIAccessSharedMemory(SHELL))
	{
	}
	//now I have access to shared memory	
	
	//write command	
	setSharedMemoryCommand(CEXIT); //ENTER COMMAND HERE

	//1st write to the data section
	//char* data = getSharedMemoryData();
	int dataSize = getSharedDataSize();
		

	//change the from
	setSharedMemoryFrom(SHELL);

	//change the too (I now don't have access)
	setSharedMemoryTo(FILEMANAGER);
	while(!canIAccessSharedMemory(SHELL))
	{
		//wait untill I hear back from the filemanager
	}
	
}

void tellCpuToExit()
{

	char to = getSharedMemoryTo();
	while(!canIAccessSharedMemory(SHELL))
	{
	}
	//now I have access to shared memory	
	
	//write command	
	setSharedMemoryCommand(CEXIT); //ENTER COMMAND HERE

	//1st write to the data section
	//char* data = getSharedMemoryData();
	int dataSize = getSharedDataSize();
		

	//change the from
	setSharedMemoryFrom(SHELL);

	//change the too (I now don't have access)
	setSharedMemoryTo(CPU);
	while(!canIAccessSharedMemory(SHELL))
	{
		//wait untill I hear back from the filemanager
	}
	
}


void program(int argc, char** argv)
{
	char to = getSharedMemoryTo();
	while(!canIAccessSharedMemory(SHELL))
	{
	}
	//now I have access to shared memory	
	
	//write command	
	setSharedMemoryCommand(CEXECUTE); //ENTER COMMAND HERE

	//1st write to the data section
	//char* data = getSharedMemoryData();
	char* data = getSharedMemoryData();
	sprintf(data, "%s\0", argv[0]);		

	//change the from
	setSharedMemoryFrom(SHELL);

	//change the too (I now don't have access)
	printf("about to switch to CPU\n");
	setSharedMemoryTo(CPU);
	while(!canIAccessSharedMemory(SHELL))
	{
		//wait untill I hear back from the filemanager
	}
	
}




void touch(int argc, char** argv)
{

	char to = getSharedMemoryTo();
	while(!canIAccessSharedMemory(SHELL))
	{
	}
	//now I have access to shared memory	
	
	//write command	
	setSharedMemoryCommand(COPEN); //ENTER COMMAND HERE

	//1st write to the data section
	char* data = getSharedMemoryData();
	sprintf(data, "%s\0", argv[1]);	
	((int*)data+(strlen(data)+1))[0] = FCREATE;
	((int*)data+(strlen(data)+1))[1] = 00755;
	

	//change the from
	setSharedMemoryFrom(SHELL);

	//change the too (I now don't have access)
	setSharedMemoryTo(FILEMANAGER);
	while(!canIAccessSharedMemory(SHELL))
	{
		//wait untill I hear back from the filemanager
	}
	data = getSharedMemoryData();

	int fd = ((int*)data)[0]; //get the file descriptor back but don't really do anything with it right now

}

void write(int argc, char** argv)
{
	int fd = -1;

	char to = getSharedMemoryTo();
        while(!canIAccessSharedMemory(SHELL))
        {
        }
        
        char* data = getSharedMemoryData();

	if(argv[1][0] == '.' && argv[1][1] == '/')//using dot slash
	{
		
		sprintf(data, "%s%s\0", path, argv[1]);
	}		
	else if( argv[1][0] == '/')//using full path
	{
		sprintf(data, "%s\0", argv[1]); //path one, source
	}
	else //using current working directory without dotslash 
	{
			sprintf(data, "%s%s\0", path, argv[1]);
	}

	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(COPEN);
	((int*)data+(strlen(data)+1))[0] = FREAD | FWRITE;
	((int*)data+(strlen(data)+1))[1] = 0;
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));
printf("1\n");
	data = getSharedMemoryData();
	fd = ((int*)data)[0];
 
	setSharedMemoryFrom(SHELL);
	setSharedMemoryCommand(CWRITE);
	((int*)data)[0] = fd;
	int size = 0; 
	int i;
	char container[500];
	for(i = 2; i < argc; i++)
	{
		strcat(container, argv[i]);
		strcat(container, " ");
		size += strlen(argv[i]) + 1;
	}
printf("2\n");
	container[size++] = '\0';
	((int*)data)[1] = size;
	sprintf(data + (sizeof(int)*2), container);
	setSharedMemoryTo(FILEMANAGER);

	while(!canIAccessSharedMemory(SHELL));
}



void rmdir(int argc, char** argv)
{
	

	char* currpath;
	currpath = calloc(256, sizeof(char));


	char to = getSharedMemoryTo();
        while(!canIAccessSharedMemory(SHELL))
        {
        }
        //now I have access to shared memory    
        
        //write command 
        
        char* data = getSharedMemoryData();
        setSharedMemoryCommand(CDELETE); //ENTER COMMAND HERE


	if(argv[1][0] == '.' && argv[1][1] == '/')//using dot slash
		{
		
			sprintf(data, "%s%s\0", path, argv[1]);
			sprintf(currpath, "%s%s\0", path, argv[1]);

		}		
		else if( argv[1][0] == '/')//using full path
		{
			sprintf(data, "%s\0", argv[1]); //path one, source
			sprintf(currpath, "%s\0", argv[1]); //path one, source
		}
		else //using current working directory without dotslash 
		{
				sprintf(data, "%s%s\0", path, argv[1]);
				sprintf(currpath, "%s%s\0", path, argv[1]);
		}

      



	int fd = -1;	

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
		int size = -1;
	
		//finding the size, moving to the end of the files data nd returning size	
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CSEEK);
		((int*)data)[0] = fd;
		((int*)data)[1] = 0;
		((int*)data)[2] = SEEKEND;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL));
		{

		}
		data = getSharedMemoryData();
		size = ((int*)data)[0]; //we know have the size... but the pointer is at the end.

		//now move the pointer back to the beginning
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CSEEK);
		((int*)data)[0] = fd;
		((int*)data)[1] = 0;
		((int*)data)[2] = SEEKSET;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL))
		{

		}
		//now the pointer is back at the beggining

		//Now read the data 
	
		//>>>>>>>>>>>>>>>>READ STREAMING DATA<<<<<<<<<<<<<<<<<<<<<<<<<<<
		int originalSize = size;
		char* buffer = calloc(originalSize + 1, sizeof(char));
		int count = 0;

		while(size > getSharedDataSize() )
		{
			//Now read the data 
			
			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CREAD);
			((int*)data)[0] = fd;
			((int*)data)[1] = getSharedDataSize();
			setSharedMemoryTo(FILEMANAGER);
		
			while(!canIAccessSharedMemory(SHELL))
			{

			}	
			data = getSharedMemoryData();
	
			//printing whats in data NEED TO SAVE THE DATA SOMWHERE
			int x;
			for (x = 0; x < getSharedDataSize(); x++)
			{
				//printf("%c", data[x]); //old
				buffer[count++] = data[x];
			}
			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CSEEK);
			((int*)data)[0] = fd;
			((int*)data)[1] = getSharedDataSize();
			((int*)data)[2] = SEEKCUR;
			setSharedMemoryTo(FILEMANAGER);

			while(!canIAccessSharedMemory(SHELL))
			{

			}
			size = size - getSharedDataSize();
		}

			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CREAD);
			((int*)data)[0] = fd;
			((int*)data)[1] = size;
			setSharedMemoryTo(FILEMANAGER);
		
			while(!canIAccessSharedMemory(SHELL))
			{

			}	
			data = getSharedMemoryData();
	
			//printing whats in data NEED TO SAVE THE DATA SOMWHERE
			int x;
			for (x = 0; x < size; x++)
			{
				buffer[count++] = data[x];
			}
		//>>>>>>>>>>>>>>>>>>>END OF REAINDING STREAMING DATA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		char* token;
		token = strtok(buffer, "#\n");
		count = 0;
		while(token != NULL)
		{
			if(count % 2 == 0)
			{	
				if(strcmp(argv[argc -1], "-a"))
				{
				//	printf("%s\n", token);
				}
				else
				{
	//	strcat(currpath, token);
					if(count > 3)
					{
						data = getSharedMemoryData();
						setSharedMemoryFrom(SHELL);
						setSharedMemoryCommand(CGETMETADATA);
						sprintf(data,"%s/%s", currpath, token);
						setSharedMemoryTo(FILEMANAGER);
						while(!canIAccessSharedMemory(SHELL))
						{

						}
						int i;
						for(i = 0; i < strlen(data); i++)
						{
							if(data[i] == '\n')
							{
								data[i] = '\t';
							}
						}
					//	printf("%s\n", data);			
					}
				}	
				count++;
				token = strtok(NULL, "#\n");
			}
			else
			{
				token = strtok(NULL, "#\n");
				count++;
			}			
		}
		if(count <= 4)
		{
			removeFile(argc, argv);	
		}
		else
		{
			printf("rmdir: failed to remove : Directory not empty\n");

		}
		free(buffer);
		free(currpath);
}
	

void cat(int argc, char** argv)
{
	int fd = -1;
	char* data;
	data = getSharedMemoryData();

	char* source;
	if(!strcmp(argv[1], "/"))
	{
		printf("you can't copy root... now going to bail\n");
	}
	else if(argv[1][0] != '/')
	{
		source = calloc( strlen(path) + strlen(argv[1])+1, sizeof(char));
		
		
		sprintf(source, "%s\0", path); //path one, sourcei
		strcat(source, argv[1]);

		sprintf(data, "%s\0", source); //path one, sourcei
	//	sprintf(data, "%s\0", argv[1]); //path one, sourcei
	}
	else
	{
		//you put in full path
		sprintf(data, "%s\0", argv[1]);
	}

	//>>>>>>>>>>>>>OPEN AND READ SOURCE FILE<<<<<<<<<<<<<<<

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
		int size = -1;
		//finding the size, moving to the end of the files data nd returning size	
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CSEEK);
		((int*)data)[0] = fd;
		((int*)data)[1] = 0;
		((int*)data)[2] = SEEKEND;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL));
		{

		}
		data = getSharedMemoryData();
		size = ((int*)data)[0]; //we know have the size... but the pointer is at the end.

		//now move the pointer back to the beginning
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CSEEK);
		((int*)data)[0] = fd;
		((int*)data)[1] = 0;
		((int*)data)[2] = SEEKSET;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL))
		{

		}
		//now the pointer is back at the beggining

		//Now read the data 
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CREAD);
		((int*)data)[0] = fd;
		((int*)data)[1] = size;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL))
		{

		}
		data = getSharedMemoryData();
	
		//printing whats in data NEED TO SAVE THE DATA SOMWHERE
		int x;
		for (x = 0; x < size; x++)
		{
			printf("%c", data[x]);
		}
		printf("\n");	
		free(source);
}

void cp(int argc, char** argv)
{
	while(!canIAccessSharedMemory(SHELL))
	{
	}
	//now I have access to shared memory	

	int fd = -1;	
	char* data;

	if(argc != 3) //check for valid number of parameters
	{
		printf("Inavlid input. Must enter a path and destination\n");	
	}
	else
	{
		//>>>>>>>>>>>>>OPEN AND READ SOURCE FILE<<<<<<<<<<<<<<<
		data = getSharedMemoryData();
	
		if(argv[1][0] == '.' && argv[1][1] == '/')//using dot slash
		{
		
			sprintf(data, "%s%s\0", path, argv[1]);
		}		
		else if( argv[1][0] == '/')//using full path
		{
			sprintf(data, "%s\0", argv[1]); //path one, source
		}
		else //using current working directory without dotslash 
		{
				sprintf(data, "%s%s\0", path, argv[1]);
		}
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
		int size = -1;
		//finding the size, moving to the end of the files data nd returning size	
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CSEEK);
		((int*)data)[0] = fd;
		((int*)data)[1] = 0;
		((int*)data)[2] = SEEKEND;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL));
		{

		}
		data = getSharedMemoryData();
		size = ((int*)data)[0]; //we know have the size... but the pointer is at the end.

		//now move the pointer back to the beginning
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CSEEK);
		((int*)data)[0] = fd;
		((int*)data)[1] = 0;
		((int*)data)[2] = SEEKSET;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL))
		{

		}
		//now the pointer is back at the beggining
		
		//>>>>>>>>>>>>>>>>READ STREAMING DATA<<<<<<<<<<<<<<<<<<<<<<<<<<<
		int originalSize = size;
		char buffer[originalSize];
		int count = 0;
		while(size > getSharedDataSize() )
		{
			//Now read the data 
			
			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CREAD);
			((int*)data)[0] = fd;
			((int*)data)[1] = getSharedDataSize();
			setSharedMemoryTo(FILEMANAGER);
		
			while(!canIAccessSharedMemory(SHELL))
			{

			}	
			data = getSharedMemoryData();
	
			//printing whats in data NEED TO SAVE THE DATA SOMWHERE
			int x;
			for (x = 0; x < getSharedDataSize(); x++)
			{
				//printf("%c", data[x]); //old
				buffer[count++] = data[x];
			}
			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CSEEK);
			((int*)data)[0] = fd;
			((int*)data)[1] = getSharedDataSize();
			((int*)data)[2] = SEEKCUR;
			setSharedMemoryTo(FILEMANAGER);

			while(!canIAccessSharedMemory(SHELL))
			{

			}
			size = size - getSharedDataSize();
		}

			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CREAD);
			((int*)data)[0] = fd;
			((int*)data)[1] = size;
			setSharedMemoryTo(FILEMANAGER);
		
			while(!canIAccessSharedMemory(SHELL))
			{

			}	
			data = getSharedMemoryData();
	
			//printing whats in data NEED TO SAVE THE DATA SOMWHERE
			int x;
			for (x = 0; x < size; x++)
			{
				buffer[count++] = data[x];
			}
		//>>>>>>>>>>>>>>>>>>>END OF REAINDING STREAMING DATA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		//>>>>CREATE NEW DESINTATION FILE<<<<<
	
		//write command	
		setSharedMemoryCommand(COPEN); //ENTER COMMAND HERE

		//1st write to the data section
		char* data = getSharedMemoryData();
		if(argv[2][0] == '.' && argv[2][1] == '/')//using dot slash
		{
				sprintf(data, "%s%s\0", path, argv[2]);
		}		
		else if( argv[2][0] == '/')//using full path
		{
			sprintf(data, "%s\0", argv[2]); //path one, source
		}
		else //using current working directory without dotslash 
		{
				sprintf(data, "%s%s\0", path, argv[2]);
		}	
	
		((int*)data+(strlen(data)+1))[0] = FCREATE;
		((int*)data+(strlen(data)+1))[1] = 00755;
	

		//change the from
		setSharedMemoryFrom(SHELL);

		//change the too (I now don't have access)
		setSharedMemoryTo(FILEMANAGER);
		while(!canIAccessSharedMemory(SHELL))
		{
			//wait untill I hear back from the filemanager
		}
		data = getSharedMemoryData();
		fd = ((int*)data)[0];
		//END OF CREATING NEW FILE
		
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>> STREAMING WRITE <<<<<<<<<<<<<<<<<<<<<<<<<<<
		//int originalSize = size;
		//char buffer[originalSize];   
		count = 0;	
		size = originalSize;
		while(size > getSharedDataSize() - 2 * sizeof(int) )
		{
			//Now read the data 
			
			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CWRITE);
			((int*)data)[0] = fd;
			((int*)data)[1] = getSharedDataSize() - 2 * sizeof(int);
			for (x =  2 * sizeof(int); x < getSharedDataSize(); x++)
			{
				//printf("%c", data[x]); //old
				//buffer[count++] = data[x];
				data[x] = buffer[count++];
			}
			setSharedMemoryTo(FILEMANAGER);
		
			while(!canIAccessSharedMemory(SHELL))
			{

			}	
			data = getSharedMemoryData();
	
			size = size - (getSharedDataSize() - 2 * sizeof(int));
		}
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CWRITE);
		((int*)data)[0] = fd;
		((int*)data)[1] = size;
		for (x = 0; x < size; x++)
		{
		
			//buffer[count++] = data[x];
			data[x +  2 * sizeof(int)] = buffer[count++];
			//memcpy(&data[x], &buffer[count++], sizeof(char));
		}
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL))
		{

		}


	}//end of else statement	
}

void mv(int argc, char** argv)
{

	char to = getSharedMemoryTo();
	while(!canIAccessSharedMemory(SHELL))
	{
	}
	//now I have access to shared memory	
	
	//write command	
	setSharedMemoryCommand(CTESTCOMMAND); //ENTER COMMAND HERE

	//1st write to the data section
	//char* data = getSharedMemoryData();
	int dataSize = getSharedDataSize();
		

	//change the from
	setSharedMemoryFrom(SHELL);

	//change the too (I now don't have access)
	setSharedMemoryTo(FILEMANAGER);
	while(!canIAccessSharedMemory(SHELL))
	{
		//wait untill I hear back from the filemanager
	}
	char* data = getSharedMemoryData();
//	printf("%s\n", data);	//if you wanna print out returned data	
	
}

void stat(int argc, char** argv)
{

	char to = getSharedMemoryTo();
	while(!canIAccessSharedMemory(SHELL))
	{
	}
	//now I have access to shared memory	
	
	//write command	
	setSharedMemoryCommand(CTESTCOMMAND); //ENTER COMMAND HERE

	//1st write to the data section
	//char* data = getSharedMemoryData();
	int dataSize = getSharedDataSize();
		

	//change the from
	setSharedMemoryFrom(SHELL);

	//change the too (I now don't have access)
	setSharedMemoryTo(FILEMANAGER);
	while(!canIAccessSharedMemory(SHELL))
	{
		//wait untill I hear back from the filemanager
	}
	char* data = getSharedMemoryData();
//	printf("%s\n", data);	//if you wanna print out returned data	
	
}

void ls(int argc, char** argv)
{
		

	while(!canIAccessSharedMemory(SHELL))
	{
	}
	//now I have access to shared memory	

	int fd = -1;	
	char* data;

	char* currpath;
	currpath = calloc(256, sizeof(char));
	if(argc < 2) //use the path
	{
		int fd = -1;
		//OPEN
		data = getSharedMemoryData();
		if(strcmp(path, "/"))
		{
			strncpy(data, path, strlen(path) -1);
			data[strlen(path) - 1] = '\0';
			sprintf(currpath, "%s\0", path);
		}
		else
		{
			sprintf(data, "%s\0", path);
			sprintf(currpath, "%s\0", path);
		}
	}
	else //specify the path	
	{	
		if(strcmp(argv[1], "-l"))
		{
			int fd = -1;
			//OPEN
			data = getSharedMemoryData();
			sprintf(data, "%s\0", argv[1]);
			sprintf(currpath, "%s\0", argv[1]);
		}
		else
		{
			int fd = -1;
			//OPEN
			data = getSharedMemoryData();
			if(strcmp(path, "/"))
			{
				strncpy(data, path, strlen(path) -1);
				data[strlen(path) - 1] = '\0';
				sprintf(currpath, "%s\0", path);
			}
				else
			{
				sprintf(data, "%s\0", path);
				sprintf(currpath, "%s\0", path);
			}
		}
	}


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
		int size = -1;
	
		//finding the size, moving to the end of the files data nd returning size	
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CSEEK);
		((int*)data)[0] = fd;
		((int*)data)[1] = 0;
		((int*)data)[2] = SEEKEND;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL));
		{

		}
		data = getSharedMemoryData();
		size = ((int*)data)[0]; //we know have the size... but the pointer is at the end.

		//now move the pointer back to the beginning
		setSharedMemoryFrom(SHELL);
		setSharedMemoryCommand(CSEEK);
		((int*)data)[0] = fd;
		((int*)data)[1] = 0;
		((int*)data)[2] = SEEKSET;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(SHELL))
		{

		}
		//now the pointer is back at the beggining

		//Now read the data 
	
		//>>>>>>>>>>>>>>>>READ STREAMING DATA<<<<<<<<<<<<<<<<<<<<<<<<<<<
		int originalSize = size;
		char* buffer = calloc(originalSize + 1, sizeof(char));
		int count = 0;

		while(size > getSharedDataSize() )
		{
			//Now read the data 
			
			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CREAD);
			((int*)data)[0] = fd;
			((int*)data)[1] = getSharedDataSize();
			setSharedMemoryTo(FILEMANAGER);
		
			while(!canIAccessSharedMemory(SHELL))
			{

			}	
			data = getSharedMemoryData();
	
			//printing whats in data NEED TO SAVE THE DATA SOMWHERE
			int x;
			for (x = 0; x < getSharedDataSize(); x++)
			{
				//printf("%c", data[x]); //old
				buffer[count++] = data[x];
			}
			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CSEEK);
			((int*)data)[0] = fd;
			((int*)data)[1] = getSharedDataSize();
			((int*)data)[2] = SEEKCUR;
			setSharedMemoryTo(FILEMANAGER);

			while(!canIAccessSharedMemory(SHELL))
			{

			}
			size = size - getSharedDataSize();
		}

			setSharedMemoryFrom(SHELL);
			setSharedMemoryCommand(CREAD);
			((int*)data)[0] = fd;
			((int*)data)[1] = size;
			setSharedMemoryTo(FILEMANAGER);
		
			while(!canIAccessSharedMemory(SHELL))
			{

			}	
			data = getSharedMemoryData();
	
			//printing whats in data NEED TO SAVE THE DATA SOMWHERE
			int x;
			for (x = 0; x < size; x++)
			{
				buffer[count++] = data[x];
			}
		//>>>>>>>>>>>>>>>>>>>END OF REAINDING STREAMING DATA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		char* token;
		token = strtok(buffer, "#\n");
		count = 0;
		while(token != NULL)
		{
			if(count % 2 == 0)
			{	
				if(strcmp(argv[argc -1], "-l"))
				{
					char *tempPath = calloc(256, sizeof(char));
					strcat(tempPath, path);
					strcat(tempPath, token);
					sprintf(data, tempPath);	
					setSharedMemoryFrom(SHELL);
					setSharedMemoryCommand(CGETPERMISSIONS);

					setSharedMemoryTo(FILEMANAGER);
					while(!canIAccessSharedMemory(SHELL))
					{

					}
					data = getSharedMemoryData();	
					int value =((int*)data)[0];
					if (value & 01000)
					{
						printf("%s%s\n",BLUE, token);			
					}
					else
					{
						printf("%s%s\n",NORMAL, token);			

					}
					printf("%s", NORMAL);	

					free(tempPath);
	
				}
				else
				{
	//	strcat(currpath, token);
					if(count > 3)
					{
						data = getSharedMemoryData();
						setSharedMemoryFrom(SHELL);
						setSharedMemoryCommand(CGETMETADATA);
						sprintf(data,"%s%s", currpath, token);
						setSharedMemoryTo(FILEMANAGER);
						while(!canIAccessSharedMemory(SHELL))
						{

						}
						int i;
						for(i = 0; i < strlen(data); i++)
						{
							if(data[i] == '\n')
							{
								data[i] = '\t';
							}
						}
						char permission[5];
						strncpy(permission, data, 4);
						permission[5] = '\0';
						int value = strtoimax(permission, NULL, 8);
							
						if (value & 01000)
						{
							printf("%s%s\n",BLUE, data);			
						}
						else
						{
							printf("%s%s\n",NORMAL, data);			

						}
						printf("%s", NORMAL);	
					}
				}	
				count++;
				token = strtok(NULL, "#\n");
			}
			else
			{
				token = strtok(NULL, "#\n");
				count++;
			}			
		}
		free(buffer);
		free(currpath);
}
void testCommand()
{
	char to = getSharedMemoryTo();
	while(!canIAccessSharedMemory(SHELL))
	{
		//wait to make sure that you can access shared memory before  you execute this command
		//you need shared memory because you are going to writing to fileManager
	}
	//now I have access to shared memory	
	//write command	
	setSharedMemoryCommand(CDEMO);

	//1st write to the data section
	//char* data = getSharedMemoryData();
	int dataSize = getSharedDataSize();
		

	//change the from
	setSharedMemoryFrom(SHELL);

	//change the too (I now don't have access)
	setSharedMemoryTo(FILEMANAGER);
	while(!canIAccessSharedMemory(SHELL))
	{
		//wait untill I hear back from the filemanager
	}
	char* data = getSharedMemoryData();

}

void help()
{
printf("\n\n>>>>>>>>>>>   HELP PAGE  <<<<<<<<<<<<<<\n");
printf("echo            :     The echo command will pipe text from standard in to standard out\n");
printf("exit            :     The exit command allows you to shut down the OS\n");
printf("cat             :     Prints the conents of a file or folder to standard out\n");
printf("cd              :     Changes the current working directory\n");
printf("cp              :     Copy's file or folder to a specified location\n");
printf("clear           :     Clears the terminal window\n");
printf("demo            :     Loads up a demo file system which can be modified and played with\n");
printf("history         :     Prints a list of all the commands issued during your current session\n");
printf("mkdir           :     Creates a directory\n");
printf("mv              :     move a file to another location\n");
printf("pwd             :     Prints the location of the current working directory\n");
printf("ls              :     List the contents of a folder. Use the \"-a\" flag for more details\n");
printf("prog[n].exe     :     Enter the command program[n] where n is an integer between 1 and 6\n                      to execute the coresponding program.\n");
printf("rm              :     Remove a file\n");
printf("rmdir           :     Remove a directory. Will only remove if the directory is empty\n");
printf("touch           :     Creates a file\n");
printf("\n");


}

