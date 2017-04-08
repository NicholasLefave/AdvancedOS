//This is our system.h file which contains things all of our processes will neeed
#ifndef SYSTEM
#define SYSTEM
#define SHMSZ 256 //how many bytes of shared memory we want
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

enum module
{
	SHELL, 
	FILEMANAGER,
	CPU,
	NOBODY
};

enum command
{
	//start of filemanager
	//-------------------------------------
	COPEN,
	CCLOSE,
	CREAD,
	CWRITE,
	CSEEK,
	CDELETE,
	CGETMETADATA,
	CGETSIZE,
	CGETPERMISSIONS,	
	CTESTCOMMAND,
	CDEMO,
	
	//start of shell
	//---------------------------------------
	CCOMMANDFAILED, //if a command fails then it will tell shell by sending thi enum	
	CDATA, //shells primary function to print whtat the other pieces of the os send it.

	//start of cpu
	//--------------------------------------
	CEXECUTE,
	CKILL,

	//Start of ALL
	//--------------------------------------
	CMOREDATA, //indicates that this data won't be null and that you should wait for more data
	CCONTINUE, //lets the sender of the person who sent the CONTINUE flag know that you got it and are ready to be sent more data
	CEXIT
};

char* getSharedMemory(); 
int canIAccessSharedMemory(enum module identifier);
char* createSharedMemory();
void setSharedMemoryTo(enum module identifier);
void setSharedMemoryFrom(enum module identifier);
void setSharedMemoryCommand(enum command identifier);
enum module getSharedMemoryFrom();
enum module getSharedMemoryTo();
enum command getSharedMemoryCommand();
char* getSharedMemoryData();
int getSharedDataSize();
int requestSharedMemory(enum module identifier);


#endif 
