#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#define SHMSZ 27 //how many byes of shared memory we want 

main()
{
	char c;
	int shmid;
	key_t key;
	char *shm;
	*s;

	key = 5678; ///unique number so the os can find the shared memory location
	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) //shmget creates the shared memory
	{
		perror("shmget"); 
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)//shmat returns the pointer to our shared memory
	{
		perror("shmat");
		exit(1);
	}
	
	*shm = '*'; //this breaks out of the sleep loop in the program above
	exit(0);	
	
}	
