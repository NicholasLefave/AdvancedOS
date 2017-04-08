#ifndef SHELL_h 
#define SHELL_h
#include <sys/shm.h> 
int shell();
char path[256];	
#endif
void moveToDepth(char* input, int depth);
