#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "a5.tab.h"
#include <fcntl.h>
#include <sys/stat.h> 
#include "cpu.h"
#include "system.h"
#include "filemanager.h"
#include <math.h>

char *yytext;
FILE *yyin;
FILE *outfile;
int yy_flex_debug;
int execute;



//int nest;

/*
int istack[10];
int istackptr=0;
int ijump=0;
int wstack[10];
int wstackptr=0;
int wjump=0;
int estack[10];
int estackptr=0;
int ejump=0;
*/


typedef struct
{ char labelname[4];
  int  pid;
  int  memloc;
} ILABEL;

typedef struct 
{ char label[5];
  int address;
} JSYM;

typedef struct 
{ char varname[11];
  int address;
} VTABLE;

VTABLE vtable[50];  // variable/address name table
int vtablex=0;
int vaddress=0;

ILABEL ilabel[10];  // store mem loc for label
int ilabelindex = 0;
int initdataindex = 0;

int initdatasection[100];
int index2initdata[20];
int index2index=0;

JSYM locjsym[30];
JSYM refjsym[30];
int refex=0;
int locex=0;

typedef struct 
{ char idop[11];
  int type;
} PARSE;

PARSE parse[100];
//PARSE parsecopy[20];
//int copysindex=0;
//int ifstck[10];

/* #define GMAX 30
char glovars[GMAX][11];
char locvars[GMAX][11];
unsigned int nglob;
*/

int yylex(void);


#define keyhit(a) {printf("hit enter --(%d)", a); getchar();}


/*
char storage[11];
char *gid[11];
char assID[11];
char prevID[11];
char optype[3];
int assIDon = 1;
*/

int i,j,kkkk;

int  sindex = 0;
int  pindex = 0;
int  memloc = 0;

#define MAXPRO 6   //max num of processes
#define MAXMEM 200 //max size of a process
#define STACKSIZE 100 //max size of the stack
#define REGISTERSIZE 10 //size of each process registers
#define MAXGMEM 20 //max size of global memory
#define NORMAL 0 //denotes a normal return from exe()
#define LOCKED 1 //stops process switching until unlock
#define UNLOCKED 2 //remove lock
#define ENDPROCESS 3
#define p0WRITE 4 //tells p0 to run-p0 should only run after a write to gmem
#define HALTING 5

#define PAGESIZE 4
#define NUMPAGES 4

int  gmem[MAXGMEM];   //global var sit here 
int mem[MAXPRO][(NUMPAGES*PAGESIZE)]; 
int endprog[MAXPRO]; // last instruction of proc
int  jsym[60]; 
int pid = 0;  //process id
int waitQueue[MAXPRO];
int page_table[MAXPRO][NUMPAGES];
int page_age[MAXPRO][NUMPAGES];
int ourfile;
int p0running;
int PageFile;

//execute one instruction return 0 if instruction is not lock
//int exe(int **stack,int sp[], int next_inst, int cur_proc);
int exe(int stack[][STACKSIZE],int sp[],int reg[][REGISTERSIZE], int next_instruct[],int next_inst[], int cur_proc);
int pop(int stack[][STACKSIZE], int proc_id, int sp[], int calledfrom);
void push(int stack[][STACKSIZE], int proc_id, int sp[],int data, int calledfrom);
void print_stack(int stack[][STACKSIZE],int sp[]); //debug
void print_register(int reg[][REGISTERSIZE]); //debug
void print_gmem();

int get_addr(int cur_proc, int address);
void age_page(int proc);
void show_age();
int getPageFile();
int registerWrite(int cur_proc, int page);

#if 0
int termcopy(int beg)
{ int i, j;
         printf("termcopy: storage loc calc here\n");
         j = 0;        
         strcpy(parsecopy[j].idop, parse[beg].idop);
         parsecopy[j].type = parse[beg].type;
 
printf("%s  ->   %s\n", parse[beg].idop, parsecopy[beg].idop);

         if(parse[beg+1].idop[0] == '[')
         { i= beg+1; j++;
           do { /* append parse[i].idop to tgt */
             strcpy(parsecopy[j].idop, parse[i].idop);
printf("%s  ->   %s\n", parse[i].idop, parsecopy[j].idop);
             parsecopy[j].type = parse[beg].type;
             j++;
           } while (parse[i++].idop[0] != ']');
         }
         copysindex = j;
}
#endif

void printArray(int array[MAXPRO][(PAGESIZE*NUMPAGES)])
{
	int st, er;
	for(st = 0; st < MAXPRO; ++st)
	{
		for(er = 0; er < PAGESIZE*NUMPAGES; ++er)
		{
			printf("%4d",array[st][er]);
		}
		printf("\n");
	}
}

/*loadmem()
{
	int ourfile = open("memfileFull", O_RDONLY | O_CREAT, 0755); //creates are file descriptor
	//read(ourfile, memFull, MAXPRO*MAXMEM*sizeof(int));
	read(ourfile, endprog, MAXPRO*sizeof(int));
	close(ourfile);
}*/


int loadmem(char *memfile)
{
	ourfile = open(memfile, O_RDONLY , 0755); //creates our file descriptor
	if(ourfile == -1)
	{	
		printf("empty file\n");
		return 0;
	}
	read(ourfile, &pid, sizeof(int));
	read(ourfile, endprog, pid*sizeof(int));
	int k, i, loadSize;
	
	for(k=0; k<pid; k++)
	{
		loadSize = endprog[k] < (PAGESIZE*NUMPAGES) ? endprog[k] : (PAGESIZE*NUMPAGES);
		for(i=0; i < loadSize; i++)
		{
			read(ourfile, &mem[k][i], sizeof(int));
			if(i%PAGESIZE == 0)
			{
				page_table[k][i/PAGESIZE] = i/PAGESIZE;
				page_age[k][i/PAGESIZE] = 0;
			}
		}
		lseek(ourfile, (endprog[k]-loadSize)*sizeof(int), SEEK_CUR);
	}
}

int cpu()
{
	char *shm;
	char *progFile;
	shm = getSharedMemory();
	int done = 0, temp;
	PageFile = getPageFile();
	while(!done)
	{
		while(!canIAccessSharedMemory(CPU));
		temp = getSharedMemoryFrom();
		if(getSharedMemoryCommand() == CEXECUTE)
		{
			shm = getSharedMemoryData();
			if(shm[0] == '/')
			{
				//our file system
			}
			else
			{
				//native file system
				progFile = calloc(strlen(shm)+1, sizeof(char));
				strcpy(progFile, shm);
				loadmem(progFile);
				memset(gmem,0,MAXGMEM*sizeof(int));
				executeit();
				free(progFile);
			}
		}
		else if(getSharedMemoryCommand() == CEXIT)
		{
			done = 1;
			setSharedMemoryFrom(CPU);
			setSharedMemoryCommand(CCLOSE);
			shm = getSharedMemoryData();
			((int*)shm)[0] = PageFile;
			setSharedMemoryTo(FILEMANAGER);
			while(!canIAccessSharedMemory(CPU));
		}
		setSharedMemoryFrom(CPU);
		setSharedMemoryTo(temp);
	}
	//loadmem();
	//printArray(mem);
	//printf("                CPU is printing %c :End Printing\n", shm[0]);//FOR TESTING REMOVE LATTER!!!! **** 
	//execute
	//executeit();
	return 0;
}

executeit()
{
  int cur_proc,p0=0, msg=-1,m;
  int stack[MAXPRO][STACKSIZE];
  int sp[MAXPRO];
  int next_instruct[MAXPRO] = {[0 ... (MAXPRO-1)] = 10};	//this line assumes gcc is the compiler
  int proc_complete[MAXPRO];
  int reg[MAXPRO][REGISTERSIZE];
  int locked=UNLOCKED;
 

  memset(stack,0,MAXPRO*STACKSIZE*sizeof(int));
  memset(sp,-1,MAXPRO*sizeof(int));
 // memset(next_instruct,0,MAXPRO*sizeof(int));
  memset(proc_complete,0,MAXPRO*sizeof(int));
  memset(reg,0,REGISTERSIZE*MAXPRO*sizeof(int));
  srand( time(NULL) );

  /*
  next_instruct[0]=10;
  next_instruct[1]=10;
  next_instruct[2]=10;
  next_instruct[3]=10;
  next_instruct[4]=10;
  next_instruct[5]=10;
  */

  /*for(i=0;i<pid;i++)
   for(m=0;m<STACKSIZE;m++)
      printf("STACK %d: %d\n",i,stack[i][m]);*/

/*while(1) //used for testing a single process
{
   cur_proc=2;
   if(next_instruct[cur_proc]<endprog[cur_proc])
   {
       msg=exe(stack,sp,reg,next_instruct,next_instruct,cur_proc);
       //increment next_instruction
      next_instruct[cur_proc]++;
      printf("loop: %d\n",next_instruct[cur_proc]);
   }
   else break;
}*/

   while(1)
   {
cont:
	  tickBlockedProcesses(waitQueue);
      if(locked==UNLOCKED)
       {// printf("pid=%d\n", pid); //keyhit(8999);
			cur_proc = (pid==1)?0:(rand()%(pid-1)+1);
			if(waitQueue[cur_proc] > 0)
			{
				printf("waiting. Ticks left: %d\n",waitQueue[cur_proc]);
				goto cont;
			}
	   }

      if(proc_complete[cur_proc] == 1)
       {
         printf("----------------------------cur_proc: %d\n",cur_proc);
             goto checkdone;
       }

      if(next_instruct[cur_proc]<endprog[cur_proc])
      {
         msg=exe(stack,sp,reg,next_instruct,next_instruct,cur_proc);
         if(msg == HALTING) return 0;

		 if(msg==ENDPROCESS)
            proc_complete[cur_proc]=1;

    //    printf("%d %d\n",cur_proc,next_instruct[cur_proc]+1);
         //increment next_instruction
         next_instruct[cur_proc]++;
         if(msg==UNLOCKED)
         {
//printf("unlock\n");
            locked=UNLOCKED;
         }
         else if(msg==LOCKED || locked==LOCKED)
         {//printf("locked\n");
            locked=LOCKED;
         }
         

        //run p0 in its entirety after a gmem write
         //cur_proc=0;
         while(msg==p0WRITE || p0running)
         {
            p0running=1; cur_proc=0;
//printf("p0 started   PC=%d\n", next_instruct[cur_proc]);
            msg=exe(stack,sp,reg,next_instruct,next_instruct,p0);
			if(msg == HALTING) return 0;
//   printf("p1, nextPC=%d\n" , next_instruct[1]);

            next_instruct[cur_proc]++;
            if(p0running == 0)
            {  msg=NORMAL;
               next_instruct[p0]=10;
               break;
            }
//printf("branch %d \n",(next_instruct[cur_proc]<endprog[cur_proc]));
            if( next_instruct[p0]>=endprog[p0])
            {  p0running=0;
               sp[p0]=0;
               next_instruct[p0]=10;
               msg=NORMAL;
               break;
            }
         }
         continue;
      }
      else
      {
           //printf("Process %d complete\n",cur_proc);
           proc_complete[cur_proc]=1;
      }
      //check if all processes are done
checkdone:
    for(cur_proc=1;cur_proc<pid;cur_proc++)
      if(proc_complete[cur_proc]==0)
         goto cont;
     break;
   }
  // print_stack(stack,sp); stack should be all 0 and sp at -1
   print_gmem();
   print_register(reg);
}

int exe(int stack[][STACKSIZE],int sp[],int reg[][REGISTERSIZE], int next_instruct[],int next_inst[], int cur_proc)
{
   int i,k, m; //delete these after all accesses renamed, except i
   int tmp,tmp1, tmp2;
   char name[11] = "aaaaaaaaaa\0";

//###############################################################################
//#								memory management								#
//###############################################################################
   i = get_addr(cur_proc, next_instruct[cur_proc]);

//printf("ENTER exe    ---  (pid=%d) PC = %d\n", cur_proc, i);
//printf("Instruction = %d\n", mem[cur_proc][i] );
//gmem[6]=101;

/** the following 3 lines are for debugging user program too **/
#if 0
   print_gmem();
   print_register(reg);
   keyhit(343);
#endif
   char* data;
   switch (mem[cur_proc][i] )
   {
     /** OPEN, READ, CLOSE, WRITE, SEEK ::  OS services **/
		   case OPEN : 
                  tmp = peek(stack,cur_proc,sp, 0) ;
                  printf("OPEN offset=  0,  data=%d\n", tmp); 
                  tmp1 = peek(stack,cur_proc,sp, -1) ;
                  printf("OPEN offset= -1,  data=%d\n", tmp1); 

                 int steven;
                 steven=0;
                 while ( name[steven] =  mem[cur_proc][get_addr(cur_proc, tmp1+steven)] )steven++;
    printf("filename passed = %s\n", name);
    printf("OS service call  --- <OPEN>  return file descriptor!\n");
		while(!requestSharedMemory(CPU));//requesting shared memory
		setSharedMemoryCommand(COPEN);//requesting FM to open
		data= getSharedMemoryData();//get the shared data which will be empty
		strcpy(data,name);//copy in name
		((int*) (data+strlen(name)+1))[0]=FREAD|FWRITE;
		((int*) (data+strlen(name)+1))[1]=0;
		setSharedMemoryFrom(CPU);//set from CPU 
		setSharedMemoryTo(FILEMANAGER);//set recipent of data after this I have given up access shared memory
		waitQueue[cur_proc]=40;//set current process to wait for 40 cycles
		while(!canIAccessSharedMemory(CPU));//wait until you can access shared memory
		tmp=((int*)data)[0];
		printf("OPEN fd: %d\n", tmp);
                push(stack,cur_proc,sp, tmp, 11); // push fd onto stack
                 
 //keyhit(91);
                break;
      case READ :
                  tmp = peek(stack,cur_proc,sp, 0) ;
                  printf("READ,  file descriptor=%d\n", tmp); 
    printf("OS service call  --- <READ> return int read (777 is fake)\n");
		while(!requestSharedMemory(CPU));//requesting shared memory
		setSharedMemoryCommand(CREAD);//requesting FM to open
		data= getSharedMemoryData();//get the shared data which will be empty
		
		((int*)data)[0] = tmp;//copy in file descriptor
		((int*)data)[1] = sizeof(int); //only reading one int
		setSharedMemoryFrom(CPU);//set from CPU 
		setSharedMemoryTo(FILEMANAGER);//set recipent of data after this I have given up access shared memory
		waitQueue[cur_proc]=40;//set current process to wait for 40 cycles
		while(!canIAccessSharedMemory(CPU));//wait until you can access shared memory
		tmp1=((int*)data)[0];//convert returned char* into int to get the int
                 push(stack,cur_proc,sp, tmp, tmp1); // dummy fd =777 

                break;

      case CLOSE :
                  tmp = peek(stack,cur_proc,sp, 0) ;
		
		
                  printf("CLOSE,  file descriptor=%d\n", tmp); 
    printf("OS service call  --- <CLOSE> close file\n");
		while(!requestSharedMemory(CPU));//requesting shared memory
		setSharedMemoryCommand(CCLOSE);//requesting FM to open
		data= getSharedMemoryData();//get the shared data which will be empty
		((int*)data)[0] = tmp;
		setSharedMemoryFrom(CPU);//set from CPU 
		setSharedMemoryTo(FILEMANAGER);//set recipent of data after this I have given up access shared memory
		waitQueue[cur_proc]=40;//set current process to wait for 40 cycles
                break;
      case WRITE :
                  tmp = peek(stack,cur_proc,sp, 0) ;
                  printf("WRITE offset=  0,  data=%d\n", tmp); 
                  tmp1 = peek(stack,cur_proc,sp, -1) ;
                  printf("WRITE offset= -1,  fd =%d\n", tmp1); 
    printf("OS service call  --- <WRITE> \n");
    /* tmp = data ; tmp1 = file descriptor */
    		
		while(!requestSharedMemory(CPU));//requesting shared memory
		setSharedMemoryCommand(CWRITE);//requesting FM to open
		data= getSharedMemoryData();//get the shared data which will be empty
		((int*)data)[0] = tmp1;
		((int*)data)[1] = sizeof(tmp);
		((int*)data)[2] = tmp;
		setSharedMemoryFrom(CPU);//set from CPU 
		setSharedMemoryTo(FILEMANAGER);//set recipent of data after this I have given up access shared memory
		waitQueue[cur_proc]=40;//set 
                break;

      case SEEK :
                  tmp = peek(stack,cur_proc,sp, 0) ;
                  printf("SEEK offset=  0,  data=%d\n", tmp); 
                  tmp1 = peek(stack,cur_proc,sp, -1) ;
                  printf("SEEK offset= -1,  fd =%d\n", tmp1); 
    printf("OS service call  --- <SEEK> \n");
    /* tmp = seek ofset ; tmp1 = file descriptor */
    		while(!requestSharedMemory(CPU));//requesting shared memory
		setSharedMemoryCommand(CSEEK);//requesting FM to open
		data= getSharedMemoryData();//get the shared data which will be empty
		((int*)data)[0] = tmp1;
		((int*)data)[1] = tmp;
		((int*)data)[2] = SEEKCUR;
		setSharedMemoryFrom(CPU);//set from CPU 
		setSharedMemoryTo(FILEMANAGER);//set recipent of data after this I have given up access shared memory
		waitQueue[cur_proc]=40;//set 
                break;



      case POPD : tmp = mem[cur_proc][get_addr(cur_proc, next_inst[cur_proc]+1)];
                  tmp1 = pop(stack,cur_proc,sp, 10) ;
//printf("POPD: popd %d into %d\n", tmp1, tmp);
                  if(tmp<230)
                  {   gmem[tmp]=tmp1;
                  }
                  else
                  {  tmp = tmp-230;
                     reg[cur_proc][tmp]= tmp1;
                  }
                  next_inst[cur_proc]++;
                  break;

      case POP : 
                  tmp1 = pop(stack,cur_proc,sp, 12) ;
                  break;

      case LD : tmp = pop(stack,cur_proc,sp, 14) ;
                tmp1 = gmem[tmp];
//printf("%04d LD %d %d\n",i,tmp1,tmp);
                push(stack,cur_proc,sp,tmp1, 15);
                break;

      case LA : tmp = mem[cur_proc][get_addr(cur_proc, next_inst[cur_proc]+1)];//load address of start of array
//printf("LA1 %d\n",tmp);
                push(stack,cur_proc,sp,tmp, 17);
//printf("%04d LA %d %d\n",i,tmp);
                  next_inst[cur_proc]++;
                break; 
      case LOAD : tmp = mem[cur_proc][get_addr(cur_proc, next_inst[cur_proc]+1)];
//printf("load 1 %d\n",tmp);
//printf("load 1 mem[%d][%d]\n",cur_proc, i+1);
//printf("stack  0= %d\n", peek(stack,cur_proc,sp, 0)) ;
//printf("stack -1= %d\n", peek(stack,cur_proc,sp, -1)) ;
                  if(tmp<230)
                     tmp1 = gmem[tmp];
                  else
                  {
                     tmp = tmp-230;
                     tmp1 = reg[cur_proc][tmp];
                  }
                  push(stack,cur_proc,sp,tmp1, 19);
//printf("%04d load tmp %d %d %d\n",i+1,tmp,tmp1,cur_proc);
//printf("stack  0= %d\n", peek(stack,cur_proc,sp, 0)) ;
//printf("stack -1= %d\n", peek(stack,cur_proc,sp, -1)) ;
                  next_inst[cur_proc]++;
                  break;
	  case LOADI:  push(stack,cur_proc,sp,mem[cur_proc][get_addr(cur_proc,next_inst[cur_proc]+1)], 21); 
//printf("%04d loadi %d\n",i,stack[cur_proc][sp[cur_proc]] );
                  next_inst[cur_proc]++;
                   break;
      case ADD: tmp1 = pop(stack,cur_proc,sp, 16);
                tmp2 = pop(stack,cur_proc,sp, 18);
                tmp1 += tmp2;
                push(stack,cur_proc,sp,tmp1, 23);
// printf("%04d:  ADD %d\n",i, tmp1); 
                break;
      case SUB : tmp1 = pop(stack,cur_proc,sp, 20);
                 tmp2 = pop(stack,cur_proc,sp, 22);
                 tmp1 = tmp2-tmp1;
                 push(stack,cur_proc,sp,tmp1, 25);
// printf("%04d:  SUB\n", i); 
                 break;
      case MUL: tmp1 = pop(stack,cur_proc,sp, 24);
                tmp2 = pop(stack,cur_proc,sp, 26);
                tmp1 *= tmp2;
                push(stack,cur_proc,sp,tmp1, 27);
   //  printf("%04d:  MUL\n", i); 
                break;
      case DIV : tmp1 = pop(stack,cur_proc,sp, 28);
                 tmp2 = pop(stack,cur_proc,sp, 30);
                 tmp1 /= tmp2;
                 push(stack,cur_proc,sp,tmp1, 29);
   //  printf("%04d:  DIV\n", i); 
                 break;
      case END: printf("Process %d completed normally\n", cur_proc);
                p0running=0;
                return ENDPROCESS;
     
case ENDP: printf("ENDP\n");
     break;

      case AND: tmp1 = pop(stack,cur_proc,sp, 32);
                tmp2 = pop(stack,cur_proc,sp, 34);
                tmp1 = tmp1 && tmp2;
                push(stack,cur_proc,sp,tmp1, 31);
 //  printf("%04d:  AND\n", i); 
                break;
      case OR: tmp1 = pop(stack,cur_proc,sp, 36);
               tmp2 = pop(stack,cur_proc,sp, 38);
               tmp1 = tmp1 || tmp2;
               push(stack,cur_proc,sp,tmp1, 33);
 //  printf("%04d:  OR\n", i); 
               break;
      case NOT: tmp1 = pop(stack,cur_proc,sp, 40);
                tmp1 = !tmp1;
                push(stack,cur_proc,sp,tmp1, 35);
 //  printf("%04d:  NOT\n", i); 
                break; 
      case LE_OP: tmp1 = pop(stack,cur_proc,sp, 42);
                  tmp2 = pop(stack,cur_proc,sp, 44);
                  tmp = tmp1 <= tmp2;
                  push(stack,cur_proc,sp,tmp, 37);
 //        printf("%04d:  LE_OP %d\n", i, tmp); 
                  break; 
      case GE_OP: tmp1 = pop(stack,cur_proc,sp, 46);
                  tmp2 = pop(stack,cur_proc,sp, 48);
                  tmp = tmp1 >= tmp2;
                  push(stack,cur_proc,sp,tmp, 39);
//    printf("%04d:  GE_OP%d\n", i,tmp); 
                  break; 
      case LT_OP:tmp1 = pop(stack,cur_proc,sp, 50);
                 tmp2 = pop(stack,cur_proc,sp, 52);
                 tmp = tmp2 < tmp1;
                 push(stack,cur_proc,sp,tmp, 41);
//    printf("%04d:  LT_OP %d %d %d\n", i,tmp,tmp1,tmp2); 
                 break;
      case GT_OP: tmp1 = pop(stack,cur_proc,sp, 54);
                  tmp2 = pop(stack,cur_proc,sp, 56);
                  tmp = tmp1 > tmp2;
                  push(stack,cur_proc,sp,tmp, 43);
//    printf("%04d:  GT_OP %d SP %d %d\n", i,tmp, sp[cur_proc],GT_OP); 
                  break;
      case EQ_OP: tmp1 = pop(stack,cur_proc,sp, 58);
//printf("step 2 %d\n",sp[cur_proc]);
                  tmp2 = pop(stack,cur_proc,sp, 60);
//printf("EQ? %d %d\n", tmp1, tmp2);
                  tmp = tmp1 == tmp2;
                  push(stack,cur_proc,sp,tmp, 45);
//     printf("%04d:  EQ_OP %d\n", i, tmp); 
                  break;
      case NE_OP: tmp1 = pop(stack,cur_proc,sp, 62);
                  tmp2 = pop(stack,cur_proc,sp, 64);
                  tmp = tmp1 != tmp2;
                  push(stack,cur_proc,sp,tmp, 47);
//     printf("%04d:  NE_OP\n", i); 
                  break; 
      case STOP : printf("STOP called by proccess %d, hit any key to continue\n", cur_proc);
                  scanf("%d",&tmp2);
                  break;
      case STOR: tmp = pop(stack,cur_proc,sp, 68);
                 tmp1 = mem[cur_proc][get_addr(cur_proc, next_inst[cur_proc]+1)];
                 if(tmp1<230)
                 {
                     gmem[tmp1]=tmp;
                     printf("Process %d wrote to global mem in index %d, %d\n",cur_proc,tmp1,gmem[tmp1]);
//printf("returning p0WRITE\n"); keyhit(99);
                  next_inst[cur_proc]++;
                     return p0WRITE;
                 } 
                 else
                 {    reg[cur_proc][tmp1-230]=tmp;
                 
                  next_inst[cur_proc]++;
                 }
                 break;
      case ST : tmp = pop(stack,cur_proc,sp, 70);//does ST ever need to store in a register?
//printf("%d\n",tmp);
                tmp1 = pop(stack,cur_proc,sp, 72);
//printf("%d\n",tmp1);
                gmem[tmp]=tmp1;
               printf("process %d wrote to global mem in index %d, %d\n",cur_proc,tmp,gmem[tmp]);
                return p0WRITE;
      case LOCK : printf("LOCK called by process %d\n",cur_proc); 
                  return LOCKED;
      case UNLOCK : printf("UNLOCK called\n");
                    return UNLOCKED;
      case HALT : printf("HALT called by process %d\n\n",cur_proc);
                  print_gmem();
                  print_register(reg);
					
				  return HALTING;
      case JFALSE : tmp=pop(stack, cur_proc,sp, 74);
                    tmp2=mem[cur_proc][get_addr(cur_proc, next_inst[cur_proc]+1)];
//printf("jfalse %d %d \n", tmp,tmp2-1);
                    if(tmp==0)
                        next_instruct[cur_proc]=tmp2-1;//sub one for PC in executeit()
                    else next_inst[cur_proc]++;
                    break;
      case JMP: tmp=mem[cur_proc][get_addr(cur_proc, next_inst[cur_proc]+1)];
                next_instruct[cur_proc]=tmp-1;//sub one for PC in executeit() 
 //    printf("%04d:  JMP\t %d\n", i, next_instruct[cur_proc]); 
                 // next_inst[cur_proc]++;
                break;
default:
     printf("illegal instruction mem[%d][%d]\n",cur_proc,i);
     printf("(%04d:   %d)\n", i, mem[cur_proc][i]);  
     keyhit(127);
     break;

   }
//printf("returning NORMAL\n"); // keyhit(9999);
   return NORMAL;
}

int peek(int stack[][STACKSIZE], int proc_id, int sp[], int offset)
{
   int val= stack[proc_id][sp[proc_id] + offset];
printf("peek : %d\n", val);
   return val;
}

int pop(int stack[][STACKSIZE], int proc_id, int sp[], int calledfrom)
{
   int val= stack[proc_id][sp[proc_id]];
   sp[proc_id]--;
   if(sp[proc_id]<-1)
   {
      printf("Stack Underflow: process %d %d\n",proc_id,sp[proc_id]);
      printf("Called from  %d\n", calledfrom);

      exit(-1);
   }
/* show stack 
   { int i, j;

     printf("pid=%d: POP Called from  %d\n", proc_id,  calledfrom);
     printf("tos = %d\n", sp[proc_id]);
     for(i=0; i<=sp[proc_id]; i++)
      printf("(%d) %d\n", i, stack[proc_id][i ]);
     printf("returning %d\n", val);
keyhit(10);
   }
*/
   return val;
}

void push(int stack[][STACKSIZE], int proc_id, int sp[],int data, int calledfrom)
{
   sp[proc_id]++;

   if(sp[proc_id]>STACKSIZE)
   {
      printf("Stack Overflow: process %d %d %d\n",proc_id,sp[proc_id],data);
      printf("PUSH Called from  %d\n", calledfrom);
      exit(-1);
   }
   stack[proc_id][sp[proc_id]]=data;

/* show stack 
   { int i, j;
     printf("pid=%d: PUSH Called from  %d\n", proc_id,  calledfrom);
     printf("tos = %d\n", sp[proc_id]);
     for(i=0; i<=sp[proc_id]; i++)
      printf("(%d) %d\n", i, stack[proc_id][i]);
keyhit(11);
   }
*/
}

//debug routines
void print_stack(int stack[][STACKSIZE],int sp[])
{
   int i,j;
   for(i=0;i<pid;i++)
   {
      printf("Stack contents for process %d\n", i);
      for(j=0;j<STACKSIZE;j++)
         printf("%d\n",stack[i][j]);
      printf("SP at %d\n\n",sp[i]);
   }
}

void print_gmem()
{
   int i;
   printf("Global memory: size %d\n",MAXGMEM);
   for(i=0;i<MAXGMEM;i++)
      printf("%d  ", gmem[i]);
   printf("\n");
}

void print_register(int reg[][REGISTERSIZE])
{
   int i,j;
   printf("Register data\n");
   for(i=0;i<pid;i++)
   {
      printf("Process %d: ",i);
      for(j=0;j<REGISTERSIZE;j++)
         printf("%d  ",reg[i][j]);
      printf("\n");

   }
   printf("returning from print_register\n");
}

//###############################################################################
//#								memory management								#
//###############################################################################
 
int page_replace(int cur_proc, int page_num);

/*
 * translate the virtual address into physical,
 * pull the page in from file if it isn't in memory
 */
int get_addr(int cur_proc, int address)
{
	int i, pageNum = address/PAGESIZE;
	age_page(cur_proc);
	for(i = 0;i < NUMPAGES; ++i)
	{
		if(page_table[cur_proc][i] == pageNum)
		{
			page_age[cur_proc][i] = 0;
			printf("page hit:cur_proc %d addr %d translated %d\n", cur_proc, address, (i * PAGESIZE) + (address % PAGESIZE));
			return (i * PAGESIZE) + (address % PAGESIZE);
		}
	}
	printf("page fault\n");
	int new_page;

	new_page = page_replace(cur_proc, pageNum); 
	
	page_age[cur_proc][new_page] = 0;
	
	return (new_page * PAGESIZE) + (address % PAGESIZE);
}

/*
 * Using an LRU page replacement algorithm, place virtual page
 * page_num into physical memory and return the physical
 * page number
 */
/*
int page_swap(int cur_proc, int page_num)
{
	int i, high = 0;
	for(i = 1; i < NUMPAGES; ++i)
	{
		if(page_age[cur_proc][high] < page_age[cur_proc][i])
			high = i;
	}
	lseek(ourfile, (pid+1)*sizeof(int), SEEK_SET);
	for(i = 0; i < cur_proc; ++i)
	{
		lseek(ourfile, endprog[i]*sizeof(int), SEEK_CUR);
	}
	lseek(ourfile, page_num*PAGESIZE*sizeof(int), SEEK_CUR);
	
	read(ourfile, &mem[cur_proc][high*PAGESIZE], PAGESIZE*sizeof(int));
	
	page_table[cur_proc][high] = page_num;
	
	printArray(mem);
	
	return high;
}*/

int page_replace(int cur_proc, int page_num)
{
	int i, high = 0;
	char * data;
	for(i = 1; i < NUMPAGES; ++i)
	{
		if(page_age[cur_proc][high] < page_age[cur_proc][i])
			high = i;
	}
	// if we need to write a register page
	registerWrite(cur_proc, page_table[cur_proc][high]);

	printf("cur_proc = %d swapping page %d for %d\n",cur_proc, page_table[cur_proc][high], page_num);

	if(page_num >= 10/PAGESIZE) //pull in from linux
	{
		lseek(ourfile, (pid+1)*sizeof(int), SEEK_SET);
		for(i = 0; i < cur_proc; ++i)
		{
			lseek(ourfile, endprog[i]*sizeof(int), SEEK_CUR);
		}
		lseek(ourfile, page_num*PAGESIZE*sizeof(int), SEEK_CUR);
	
		read(ourfile, &mem[cur_proc][high*PAGESIZE], PAGESIZE*sizeof(int));

	}
	if(page_num <= 10/PAGESIZE) //pull from our file system
	{
		//lseek(ourfile, page_num*PAGESIZE*sizeof(int), SEEK_CUR);
		while(!requestSharedMemory(CPU));
		setSharedMemoryCommand(CSEEK);
		setSharedMemoryFrom(CPU);
		data = getSharedMemoryData();
		((int*)data)[0] = PageFile;
		((int*)data)[1] = cur_proc*10*sizeof(int) + page_num*PAGESIZE*sizeof(int);
		((int*)data)[2] = SEEKSET;
		setSharedMemoryTo(FILEMANAGER);
		
		while(!canIAccessSharedMemory(CPU));
		//read(ourfile, &mem[cur_proc][high*PAGESIZE], PAGESIZE*sizeof(int));
		int Size = page_num == 10/PAGESIZE ? (10%PAGESIZE)*sizeof(int) : PAGESIZE*sizeof(int);
		setSharedMemoryCommand(CREAD);
		setSharedMemoryFrom(CPU);
		((int*)data)[0] = PageFile;
		((int*)data)[1] = Size;
		setSharedMemoryTo(FILEMANAGER);
	
		while(!canIAccessSharedMemory(CPU));
		memcpy(&mem[cur_proc][high*PAGESIZE], data, Size);

	}
	page_table[cur_proc][high] = page_num;
	
	
	return high;
}

int registerWrite(int cur_proc, int page)
{
	//write the page to be swapped out to pageFile
	if(page > 10/PAGESIZE) return 0;

	printf("writing registers\n");
	int iter;
	for(iter = 0; iter < 10; ++iter)
		printf("%d ",mem[cur_proc][iter]);
	printf("\n");
	while(!requestSharedMemory(CPU));
	setSharedMemoryCommand(CSEEK);
	setSharedMemoryFrom(CPU);
	char *data = getSharedMemoryData();
	((int*)data)[0] = PageFile;
	((int*)data)[1] = cur_proc*10*sizeof(int) + page*PAGESIZE*sizeof(int);
	((int*)data)[2] = SEEKSET;
	setSharedMemoryTo(FILEMANAGER);
	
	while(!canIAccessSharedMemory(CPU));
	setSharedMemoryCommand(CWRITE);
	setSharedMemoryFrom(CPU);
	((int*)data)[0] = PageFile;
	//size to write is smaller if we don't have a full page to write, which happens at the end of the registers
	((int*)data)[1] = page == 10/PAGESIZE ? (10%PAGESIZE)*sizeof(int) : PAGESIZE*sizeof(int);
	memcpy(data + sizeof(int)*2, &mem[cur_proc][page*PAGESIZE], PAGESIZE*sizeof(int));
	setSharedMemoryTo(FILEMANAGER);

	return 1;
}

/*
 * age all of the pages for the given process
 *
 * @param proc: the process id we need to age
 */
void age_page(int proc)
{
	int i;
	for(i = 0; i < NUMPAGES; ++i)
	{
		++page_age[proc][i];
	}
}

/*
 * display all the ages for the LRU replacement
 */
void show_age()
{
	int i, j;
	for(i = 0; i < MAXPRO; ++i)
	{
		printf("process %d: ", i);
		for(j = 0; j < NUMPAGES; ++j)
		{
			printf("%4d ", page_age[i][j]);
		}
		printf("\n");
	}
}

int getPageFile()
{
	while(!requestSharedMemory(CPU));
	setSharedMemoryFrom(CPU);
	setSharedMemoryCommand(COPEN);
	char *data = getSharedMemoryData();
	sprintf(data, "/.pageFile\0");
	((int*)data+(strlen(data)+1))[0] = FCREATE|FREAD|FWRITE;
	((int*)data+(strlen(data)+1))[1] = 00700;

	setSharedMemoryTo(FILEMANAGER);//file manager now owns shared memory
	while(!canIAccessSharedMemory(CPU));
	data = getSharedMemoryData();
	int fd = ((int*)data)[0]; 
	setSharedMemoryCommand(CWRITE);
	((int*)data)[1] = sizeof(int)*10*MAXPRO;
	setSharedMemoryFrom(CPU);
	setSharedMemoryTo(FILEMANAGER);
	while(!canIAccessSharedMemory(CPU));
	setSharedMemoryTo(NOBODY);
	return fd; 
}

//#######################################################################
//							Process Scheduling
//#######################################################################

int tickBlockedProcesses(int * processes)
{
	int i;
	for(i = 0; i < pid; ++i)
	{
		if(processes[i] > 0)
			--processes[i];
	}
}


