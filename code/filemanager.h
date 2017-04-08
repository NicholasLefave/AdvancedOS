#ifndef FILEMANAGER_h 
#define FILEMANAGER_h
#include <sys/shm.h> 
int fileManagerInit();
int fileManagerShutdown();

int Open(char *path, int flags, int mode);
int Close(int fd);
int Read(int fd, void *buffer, unsigned int size);
int Write(int fd, void *buffer, unsigned int count);
int seek(int fd, int offset, int whence);
int delete(char *path);
int getMetaData(char *path, char from);
int getSize(int fd);
int getFilePermissions(char *path);
int fileManagerIsRunning();
void checkSharedMemory();
void demoPopulate();

//constants
#define FCREATE 0x4
#define FREAD 0x1
#define FWRITE 0x2
#define APPEND 0x8
#define FTRUNCATE 0x10

#define SEEKSET 0x1
#define SEEKCUR 0x2
#define SEEKEND 0x4

#endif
