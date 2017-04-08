#include "system.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *shm;
int shmid;
key_t key = 5678; ///unique number so the os can find the shared memory location

int getSharedDataSize()
{
	return SHMSZ - 3;
}

void readSharedData()
{

}
//////////GET SHARED MEMROY ///////////////////////////////////
// Returns a pointer to where shared meory is located (assuming it's already been created)
/////////////////////////////////////////////////////////////
char* getSharedMemory() 
{

	//Set up IPC
	 if ((shmid = shmget(key, SHMSZ, 0666)) < 0) //shmget opens the shared memory
        {
                perror("shmget");
                exit(1);
        }

        if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)//shmat returns the pointer to our shared memory
        {
                perror("shmat");
                exit(1);
        }
	
	return shm;
}

///////////////// Can I Access Shared Memory /////////////////////
// This checks the first slot in our shared memory to see if the calling class has access to the message portion of shared memory. Returns 0 for true and 1 for false.
//
///////////////////////////////////////////////////////////////////
int canIAccessSharedMemory(enum module identifier)
{
	if(shm[0] == (char)identifier)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

///////////////// Request shared memory  /////////////////////
// like canIAccessSharedMemory
///////////////////////////////////////////////////////////////////
int requestSharedMemory(enum module identifier)
{
	if(shm[0] == (char)identifier || shm[0] == (char)NOBODY)
	{
		shm[0] = (char)identifier;
		return 1;//true
	}
	else
	{
		return 0;//false
	}
}


////////////////////////Get Shared Memory Data///////////////////////////
// returns a pointer the beginning of the shared memory section
// use this in tandom with the getSharedMemoryDataSize() function
////////////////////////////////////////////////////////////////////////
char* getSharedMemoryData()
{
	//use printf to print the whats stored in this memory location to the screen
	//use sprintf to write to the shared memory (but be sure you don't go beyond the bounds
	return &shm[3];
}

//////////////////////Set Shared Memory Command///////////////////
//Sets the 'Command' section of our shared meory
//////////////////////////////////////////////////////////////////
void setSharedMemoryCommand(enum command identifier)
{
	shm[2] = (char)identifier;
}

enum command getSharedMemoryCommand()
{
	return (enum command)shm[2];
}
/////////////////// Set Shared Memory To /////////////////////////
// Sets the 'To' section of our shared memory
////////////////////////////////////////////////////////////////
void setSharedMemoryTo(enum module identifier)
{
	shm[0] = (char)identifier;
}

enum module getSharedMemoryTo()
{
	return (enum module)shm[0];
}


////////////////// Set Shared Memory From //////////////////////
// Sets the 'From' section of our shared memory 
///////////////////////////////////////////////////////////////
void setSharedMemoryFrom(enum module identifier)
{
	shm[1] = (char)identifier;	
}

enum module getSharedMemoryFrom()
{
	return (enum module)shm[1];
}

//////////////////CREATE SHARED MEMORY/////////////////////
// Creates a new space for shared memory and 'null' writes it incase anything is still there from previous runs.
// Note: Boot is the only one who should ever call this function. Everyone else should use getSharedMemory()
/////////////////////////////////////////////////////////
char* createSharedMemory()
{
	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) //shmget creates the shared memory
	{
		perror("shmget");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)//shmat returns the pointer to our shared memory
	{
		perror("shmat");
		exit(1);
	}

	//Clear the memory before we use it, makes sure we don't have old values left over
	int j;
	for(j=0; j < SHMSZ; j++)
	{
		shm[j] = '\0';
	}
	
	return shm;
}
