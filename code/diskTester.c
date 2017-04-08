#include "disk.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	diskFormat();
	char *testArray[16];

	printf("%p\n",testArray[0]);
	//fill the disk
	testArray[0] = getSpace(DISK_SIZE);
	*testArray[0] = 3;
	printf("%p\n",testArray[0]);
	//try to get space from full disk
	printf("%p\n", testArray[1]);
	testArray[1] = getSpace(2);
	printf("%p\n",testArray[1]);

	//delete from front of full disk
	int ret = deleteSpace(testArray[0], 20);	
	//delete from back of disk	
	ret = deleteSpace(testArray[0] + DISK_SIZE - 20, 20);
	//delete from side to cause shift
	ret = deleteSpace(testArray[0] + 20, 20);
	ret = deleteSpace(testArray[0] + DISK_SIZE - 40, 20);
	//delete and merge 
	ret = deleteSpace(testArray[0] + 20, DISK_SIZE -40);
	
	writeOutFreeList();
	return 0;
}

