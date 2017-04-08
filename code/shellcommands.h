//Shell Commands.h -- after you put a command in shellcommands.c remember to put it in here
void echo();
void help();
void ls(int argc, char** argv);
void mkdir(int argc, char** argv);
void touch(int argc, char** argv);
//void rm(int argc, char** argv);
void rmdir(int argc, char** argv);
void cat(int argc, char** argv);
void write(int argc, char** argv);
void mv(int argc, char** argv);
void stat(int argc, char** argv);
void program(int argc, char** argv);
void testCommand();
void tellFileManagerToExit();
void tellCpuToExit();
