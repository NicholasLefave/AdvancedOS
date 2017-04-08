#ifndef DISK_h
#define DISK_h

#define DISK_SIZE 0x8000

char disk[DISK_SIZE];

void diskFormat();
char* getSpace(long size);
char* extendSpace(char* start, long currSize, long newSize);
int deleteSpace(char* start, long size);
int writeOutFreeList();
void deleteDisk();

#endif
