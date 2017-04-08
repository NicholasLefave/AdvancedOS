%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "tpl.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

char *memfile;
char *yytext;
FILE *yyin;
FILE *outfile;
int yy_flex_debug;



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
void yyerror(char *);


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
int  gmem[MAXGMEM];   //global var sit here 
int mem[MAXPRO][MAXMEM]; 
int endprog[MAXPRO]; // last instruction of proc
int  jsym[60]; 
int pid = 0;  //process id

  int p0running;

int showparse( int beg)
{ int i;
        for (i=beg; i<sindex; i++)
        {  printf("############## i=%d  %s, (type)%d\n",
                                i, parse[i].idop, parse[i].type);
        }
        printf("end show\n");
}


int showterm( int beg)
{ int i;
        return;

        for (i=beg; i<sindex; i++);
}


%}

%union
{int intval;
 char *sptr;
}

%token NUMBER
%token ID
%token LD 
%token LA 
%token LOAD 
%token LOADI 
%token ADD
%token SUB 
%token MUL 
%token DIV 
%token END
%token ENDP
%token PROCESS
%token AND
%token OR
%token NOT
%token LE_OP
%token GE_OP
%token LT_OP
%token GT_OP
%token EQ_OP
%token NE_OP
%token STOP 
%token STOR
%token ST 
%token DATA 
%token IDATA 
%token COMMA 
%token COLON 
%token LOCK 
%token UNLOCK 

%token OPEN 
%token READ 
%token WRITE 
%token CLOSE 
%token SEEK 
%token POPD 
%token POP 

%token DOT 
%token HALT 
%token JFALSE 
%token JMP 

%left  LPREC 
%right RPREC 

%%

programA: program
{
	int j,k;
	printf("END of ProgramA\n");
	for(j=0; j<ilabelindex; j++)
	{
		printf("pid = %d\n", ilabel[j].pid );
		printf("memloc = %d\n", ilabel[j].memloc);
		printf("name = %s\n", ilabel[j].labelname);
	}
};

program: program0 
	| varlistend  program0  {; }
	| program  program0  {;}
	;

varlistend: dotdata varlists
	{;}
        ;

dotdata: DOT DATA 
	{ sindex=0;
	}
	;

varlists: varlists varlist
	| varlist
	;

varlist: term 
{ 
	int i;
	//for (i=0; i<sindex; i++)
	i=0;
	{ 
	strcpy(vtable[vtablex].varname, parse[i].idop);
	vtable[vtablex].address=vaddress;
	vtablex++;
	vaddress++;
	showsymtable();
	//keyhit(9);
	}
	gmem[memloc++]=0;
	//showterm(0);
	sindex = 0;
}
	| term COMMA term 
{ 
	int i,j;

	printf("term comma term\n");
	showparse(0);
	printf("vtablex=%d vaddress:%d\n", vtablex, vaddress);
	strcpy(vtable[vtablex].varname, parse[0].idop);

	vtable[vtablex].address=vaddress;
	sscanf(parse[2].idop, "%d", &i);

	printf("vtable[%d].varname=%s, parse[0].idop=%s\n", 
	vtablex, vtable[vtablex].varname,    parse[0].idop);
	printf(" vtable[vtablex].address=%d\n", vtablex, vtable[vtablex].address);
	//keyhit(99);

	vaddress += i;
	vtablex++;

	showsymtable();
	printf("vtablex=%d vaddress:%d\n", vtablex, vaddress);
	//keyhit(10);

	for(j=0; j<i; j++)
	{  
		gmem[memloc++]=0;
	}
	//showterm(0);
	sindex = 0;
}
        ;

program0: program1
	| program1 initdata
	; 

initdata: dotidata labels
	{ //printf("inidata\n");
	//showparse(0);
	}
	|
	dotidata
	{
	}
	;
dotidata: DOT IDATA
	{ sindex=0;
	}
	;

labels: labels labl collabeldata
	| labl collabeldata
	{// printf("labels\n");
	//showparse(0);
	}
        ;

labl: term
         { //printf("labl\n");
           //showparse(0);
         }
        ;

collabeldata: COLON term
         { int i, j, k;
           int ch; 
           char *chptr;

           printf("collabeldata\n");
           showparse(0);
	printf("==================================\n");
	printf("parse[0].idop=%s\n", parse[0].idop);
	printf("parse[2].idop=%s\n", parse[2].idop);

          for(j=0; j<ilabelindex; j++)
          {
           printf("pid = %d\n", ilabel[j].pid );
           printf("memloc = %d\n", ilabel[j].memloc);
           printf("name = %s\n", ilabel[j].labelname);

           if( ilabel[j].memloc != -999)
           { printf("compare ilabel's %s  and label parsed %s\n",
             ilabel[j].labelname,  parse[0].idop); 
             if( strcmp(ilabel[j].labelname ,  parse[0].idop)==0)
             { printf("label found, endprog is at  %d\n",
                  endprog[ ilabel[j].pid] );

               chptr =  parse[2].idop;
               for( ; *chptr; chptr++) 
               { printf("%c", *chptr);
               }
                printf("\n");

               chptr =  parse[2].idop;
               mem[ ilabel[j].pid][ ilabel[j].memloc ] = 
                                                  endprog[ilabel[j].pid];
               for( ; *chptr; chptr++) 
               { printf("at %d: locating '%c'\n", 
                  endprog[ilabel[j].pid], *chptr);
                  mem[ilabel[j].pid][endprog[ilabel[j].pid]] = *chptr;
                  endprog[ilabel[j].pid]++;
               }
               mem[ ilabel[j].pid][endprog[ilabel[j].pid]] = 0;
               endprog[ilabel[j].pid]++;
               ilabel[j].memloc = -999;
             } 
           }

          }


	printf("==================================\n");
	printf("==================================\n");
	for(j=0; j<ilabelindex; j++)
	printf("pid = %d  end lo = %d\n", ilabel[j].pid, endprog[ilabel[j].pid]);
	printf("==================================\n");

   // allocate parse[2].idop to initdatasection[]
   // and backpatch Li labels. 
//ILABEL ilabel[10];  // store mem loc for label
//int ilabelindex = 0;
/*typedef struct
{ char labelname[4];
  int  pid;
  int  memloc;
} ILABEL;
*/      

	printf("index2initdata[%d] =  %d\n", index2index, initdataindex);       
	index2initdata[index2index]=initdataindex;       
	for (k=0; ch = *((char *)(parse[2].idop)+k); k++)
	initdatasection[initdataindex++]= ch;
	index2index++;
 
        for(k=0; k<initdataindex; k++)
             printf("%c", initdatasection[k]);
        printf("\n");
        for(k=0; k<index2index; k++)
             printf("%d  ", index2initdata[k]);
	printf("\n==================================\n");
            sindex = 0;
         }
        ;


program1:   process0 code0 END 
	{
                 mem[pid][memloc++]=END;
                 sindex=0;
                 endprog[pid]=memloc; // last instruction location+1
		 printf("at END refex=%d locex=%d\n", refex, locex);
                 showjsym(); 
                 refex = locex = 0;
                 pid++;
               }
        ;

process0:
           PROCESS term 
              {
                //printf("PROC\n");
                showterm(0);
                sindex = 0; 
                memloc = 10;
                if(pid>MAXPRO) 
                   {fprintf(stderr,"max num of proccesses currently 6\n");
                    exit(1);
                   }
              }

        ;

code0:
         code0 code
         | code
      ;

code:    LOAD term
           {int i, j;
            mem[pid][memloc++]=LOAD;
             i=searchvtable(parse[0].idop, &j, 1);
             if( i>=230 )
             {
               mem[pid][memloc++]=i;
             }
             else
             {
       printf("vtable[%d].address=%d\n",i, vtable[i].address);
    //   keyhit(543);
               mem[pid][memloc++]=vtable[i].address;
             }
             showterm(0);
             sindex = 0;
           }
       | LOADI term 
           {int i;
            mem[pid][memloc++]=LOADI;
             sscanf(parse[0].idop, "%d", &i);
            mem[pid][memloc++]=i;
             showterm(0);
             sindex = 0;
           }
       | LA term 
           { int i, j;
            mem[pid][memloc++]=LA;
	    printf("LA term parse[0].idop = %s\n",parse[0].idop); 
            i=searchvtable(parse[0].idop, &j, 2);
	    printf("LA term searchvtable ret val= %d\n", i); 
	    //keyhit(645);
             if( i == -999)
             { ilabel[ilabelindex].pid = pid;
               ilabel[ilabelindex].memloc = memloc;
               sprintf(ilabel[ilabelindex].labelname,
                 "%s",  parse[0].idop);
               ilabelindex++;
               mem[pid][memloc++]= -999;
printf("wrote mem[%d][%d] =  %d\n", pid, memloc-1, mem[pid][memloc-1]); 
/**
{ int i,j;
  printf("ilabelindex=%d\n",ilabelindex);
          for(j=0; j<ilabelindex; j++)
          {
           printf("pid = %d\n", ilabel[j].pid );
           printf("memloc = %d\n", ilabel[j].memloc);
           printf("name = %s\n", ilabel[j].labelname);
          }
          keyhit(645);
}
**/
             }
             else
             {
               //mem[pid][memloc++]=j;
               mem[pid][memloc++]=i;
             }
             showterm(0);
             sindex = 0;
           }
       | OPEN
          {
            mem[pid][memloc++]=OPEN;
                showterm(0);
                sindex = 0;
          }
       | READ
          {
            mem[pid][memloc++]=READ;
                showterm(0);
                sindex = 0;
          }
       | WRITE
          {
            mem[pid][memloc++]=WRITE;
                showterm(0);
                sindex = 0;
          }
       | CLOSE
          {
            mem[pid][memloc++]=CLOSE;
                showterm(0);
                sindex = 0;
          }
       | SEEK
          {
            mem[pid][memloc++]=SEEK;
                showterm(0);
                sindex = 0;
          }
       | POPD term
           {int i, j;
                showterm(0);
            mem[pid][memloc++]=POPD;
             i=searchvtable(parse[0].idop, &j, 31);
             if( i>=230 )
             { // 230 and up is local var r?
               mem[pid][memloc++]=i;
             }
             else
             {
               mem[pid][memloc++]=vtable[i].address;
             }

                showterm(0);
                sindex = 0;
           }
       | POP
          {
            mem[pid][memloc++]=POP;
                showterm(0);
                sindex = 0;
          }

       | JFALSE term 
           { 
            mem[pid][memloc++]=JFALSE;
            mem[pid][memloc]=-1;
             //printf("label =  %s  %d\n", yylval.sptr, sindex);  
                refjsym[refex].address = memloc++;
                sprintf( refjsym[refex].label, "%s", parse[0].idop);
                refex++;
             showterm(0);
             sindex = 0;
           }
       | JMP term 
           {
            mem[pid][memloc++]=JMP;
            // printf("label =  %s  %d\n", yylval.sptr, sindex);  
            mem[pid][memloc]=-1;
                refjsym[refex].address = memloc++;
                sprintf( refjsym[refex].label, "%s", parse[0].idop);
                refex++;
                showterm(0);
                sindex = 0;
           }
       | LOCK  
           {
            mem[pid][memloc++]=LOCK;
                showterm(0);
                sindex = 0;
           }
       | UNLOCK  
           {
            mem[pid][memloc++]=UNLOCK;
                showterm(0);
                sindex = 0;
           }
       | LD  
           {
            mem[pid][memloc++]=LD;
                showterm(0);
                sindex = 0;
           }
       | ST  
           {
            mem[pid][memloc++]=ST;
                showterm(0);
                sindex = 0;
           }
       | STOR term  
           {int i, j;
                showterm(0);
            mem[pid][memloc++]=STOR;
             i=searchvtable(parse[0].idop, &j, 3);
//printf("STOR %d\n", i); keyhit(89);
             if( i>=230 )
             { // 230 and up is local var r?
               mem[pid][memloc++]=i;
             }
             else
             {
               //mem[pid][memloc++]=vtable[i].address;
               mem[pid][memloc++]=i;
             }

                showterm(0);
                sindex = 0;
           }
       | EQ_OP 
           {
            mem[pid][memloc++]=EQ_OP;
                showterm(0);
                sindex = 0;
           }
       | GT_OP 
           { 
            mem[pid][memloc++]=GT_OP;
                showterm(0);
                sindex = 0;
           }
       | GE_OP 
           {
            mem[pid][memloc++]=GE_OP;
                showterm(0);
                sindex = 0;
           }
       | LT_OP 
           {
            mem[pid][memloc++]=LT_OP;
                showterm(0);
                sindex = 0;
           }
       | LE_OP 
           {
            mem[pid][memloc++]=LE_OP;
                showterm(0);
                sindex = 0;
           }
       | ADD 
           {
            mem[pid][memloc++]=ADD;
                showterm(0);
                sindex = 0;
           }
       | SUB 
           {
            mem[pid][memloc++]=SUB;
                showterm(0);
                sindex = 0;
           }
       | AND 
           {
            mem[pid][memloc++]=AND;
                showterm(0);
                sindex = 0;
           }
       | OR 
           {
            mem[pid][memloc++]=OR;
                showterm(0);
                sindex = 0;
           }
       | HALT 
           {
            mem[pid][memloc++]=HALT;
                showterm(0);
                sindex = 0;
           }
       | term COLON 
           {
                locjsym[locex].address = memloc;
                sprintf( locjsym[locex].label, "%s", parse[0].idop);
                locex++;
                showterm(0);
                sindex = 0;
           }
      ;


term:   
         ID
            {
              sprintf( parse[sindex].idop, "%s", yylval.sptr);
              parse[sindex].type = ID;
              sindex++;


            }
        | NUMBER 
            {
              sprintf( parse[sindex].idop, "%s", yylval.sptr);
              parse[sindex].type = NUMBER;
              sindex++;
            }

        ;



%%

void yyerror(char *s) {
 extern unsigned int linenumber;
    fprintf(stderr, "%s\n", s);
    fprintf(stderr, "line %d:  %s\n", linenumber+1, yytext);
    return;
}

savemem()
{
	int ourfile = open("memfileFull", O_WRONLY | O_CREAT | O_TRUNC, 0755); //creates are file descriptor
	write(ourfile, mem, MAXPRO*MAXMEM*sizeof(int));
	write(ourfile, endprog, MAXPRO*sizeof(int));
	close(ourfile);
}

printmem()
{
 	int ourfile = open(memfile, O_WRONLY | O_CREAT | O_TRUNC, 0755); //creates are file descriptor
	write(ourfile, &pid, sizeof(int));
	write(ourfile, endprog, pid*sizeof(int));//changed from maxpro to pid
	int k;
	int i;
	for(k=0; k<pid; k++)
	{
		for(i=0; i<endprog[k]; i++)
 		{ 
			write(ourfile, &mem[k][i], sizeof(int));
  		}
	} // end of printing each proc memory
	
	
	close(ourfile);

}

main(int argc, char **argv )
{   
	
	if( argc != 3 )
	{ 
		fprintf(stderr, "bad command\n");
		exit(0);  //  yyin = stdin;
	}
	memfile = calloc(strlen(argv[2]), sizeof(char));
	strcpy(memfile, argv[2]); 
	yyin = fopen( argv[1], "r" );

	yyparse();

	fclose(yyin);

	showjsym(); 
	showsymtable();
	savemem();
	printmem(); //--use this instead of save mem - this is the correct way
//	showmem();
	free(memfile);
	return 0;
}


showsymtable()
{int i;
     for (i=0; i<vtablex; i++)
     {  printf("%d:  %s\n",
        vtable[i].address,  vtable[i].varname);
     }
}

int searchvtable( char *str, int *j, int from)
{int i, found;


printf("--------search vtable for %s  from=%d\n", str, from);
printf("%c %c\n", *str, *(str+1));
//keyhit(888);

           if( *str =='r' &&
              ( '0' <= *(str+1) && *(str+1) <= '9' ))
           { 
printf("this is local var\n");
                 // 230 or above is locar var, r?
                 return( (*(str+1) - '0')+230 ); 

           }

             for(i=found=0; i<vtablex; i++)
             {
//printf("comp against : %s\n", vtable[i].varname);
               if(strcmp(vtable[i].varname, str)==0)
               { found =1; *j = i; break;}
             }
             if(found)
             {;
             }
             else
             {
              // fprintf(stderr, "%d: var %s not found\n", from, str );
               return (-999);
             }
printf("---------search vtable for %s ended\n", str);
printf("----------returning %d   and  j= %d\n", vtable[i].address, *j);
   return( vtable[i].address);
}

showjsym()
{
  int i, j;
  for(i=0; i<locex; i++)
  {
    printf("%s  %d\n",locjsym[i].label, locjsym[i].address);
  }

  printf("SHOWSYM refex %d\n", refex);
  for(i=0; i<refex; i++)
  {
    printf("%s  %d", refjsym[i].label, refjsym[i].address );
    printf(" --  %d\n", mem[pid][ refjsym[i].address] );
  }
  /* search refjsym in locjsym, fill with locjsym.address */  
  for(i=0; i<refex; i++)
  {
    printf("%s  %d", refjsym[i].label, refjsym[i].address );
    printf(" --  %d\n", mem[pid][ refjsym[i].address] );
    /* search refjsym in locjsym, fill with locjsym.address */  
    if( mem[pid][ refjsym[i].address] != -1)
    printf("jump lable wrong %s\n", refjsym[i].label);
    for(j=0; j<locex; j++)
    {
      //   printf("loc:%s  ref:%s\n",  locjsym[j].label,  refjsym[i].label);
      if( strcmp(locjsym[j].label,  refjsym[i].label)== 0 )
      {
	// printf("patch = %d\n", locjsym[j].address);
	mem[pid][ refjsym[i].address] = locjsym[j].address; 
	break;
      }
    }
    if(j>=locex)
    fprintf(stderr,"lable not found: %s\n", refjsym[i].label);
  }
  printf("DONE showsym\n");  
}

int searchsym( char *str, int j)
{int i;
/********
          { int i, k;
            printf("look for loc j=%d\n", str, j);
            for (i=0; i<vtablex; i++)
            {  printf("vtable[%d].addres= %d .EQ. j=%d , (%s)\n",
                    i, vtable[i].address, j,  vtable[i].varname);
            }
          }
     printf("that's end of vtable\n");          
     keyhit(1122);
**********/

             for(i=0; i<vtablex; i++)
             {
//printf("searchsym::: comp %d against : %d\n",  j, vtable[i].address);
               if(vtable[i].address == j )
               { sscanf(vtable[i].varname, "%s", str);
                 return; 
               }
             }
             { fprintf(stderr, " symbol %s not found, error\n", str);
               fprintf(stderr, " this may be initdata label.\n");
             }
printf("---------search vtable for %s ended\n", str);
}


showmem()
{ int i, k; char name[11];
  int progid;
  

for(k=0; k<pid; k++)
{
printf("############### pid %d ############\n", k);
printf("############### endprog %d ############\n", endprog[k]);
            // endprog[] contains last instruction addrs of proc
  for(i=0; i<endprog[k]; i++)
  { // printf("%04d:    %d\n", i,   mem[k][i]);
    switch (mem[k][i] )
    {
/*
case NUMBER:
     printf("  %d\n",  mem[k][i]); 
     break;
case ID:
     printf("  %d\n",  mem[k][i]); 
     break;
*/
case POPD :
     if(mem[k][i+1]<230)
     { searchsym(name, mem[k][i+1]);
       printf("%04d:  POPD\t %d  (%s)\n", i, mem[k][i+1], name);
     }
     else
       printf("%04d:  POPD\t %d\n", i, mem[k][i+1]); 
     i++;
     break;
case POP :
     printf("%04d:  POP\n", i); 
     break;
case OPEN :
     printf("%04d:  OPEN\n", i); 
     break;
case READ :
     printf("%04d:  READ\n", i); 
     break;
case WRITE :
     printf("%04d:  WRITE\n", i); 
     break;
case SEEK :
     printf("%04d:  SEEK\n", i); 
     break;
case CLOSE :
     printf("%04d:  CLOSE\n", i); 
     break;
case LD :
     printf("%04d:  LD\n", i); 
     break;
case LA :
     searchsym(name, mem[k][i+1]);
     printf("%04d:  LA\t %d (%s)\n", i, mem[k][i+1], name); i++;
     break;
case LOAD :
     if(mem[k][i+1]<230)
     { searchsym(name, mem[k][i+1]);
       printf("%04d:  LOAD\t %d  (%s)\n", i, mem[k][i+1], name);
     }
     else
       printf("%04d:  LOAD\t %d\n", i, mem[k][i+1]); 
     i++;
     break;
case LOADI :
     printf("%04d:  LOADI\t %d\n", i, mem[k][i+1]); i++;
      break;
case ADD:
     printf("%04d:  ADD\n", i); 
     break;
case SUB :
     printf("%04d:  SUB\n", i); 
     break;
case MUL :
     printf("%04d:  MUL\n", i); 
     break;
case DIV :
     printf("%04d:  DIV\n", i); 
     break;
case END:
     printf("%04d:  END\n", i); 
     break;
case ENDP:
     printf("%04d:  ENDP\n", i); 
     break;
case AND:
     printf("%04d:  AND\n", i); 
     break;
case OR:
     printf("%04d:  OR\n", i); 
     break;
case NOT:
     printf("%04d:  NOT\n", i); 
     break;
case LE_OP:
     printf("%04d:  LE_OP\n", i); 
     break;
case GE_OP:
     printf("%04d:  GE_OP\n", i); 
     break;
case LT_OP:
     printf("%04d:  LT_OP\n", i); 
     break;
case GT_OP:
     printf("%04d:  GT_OP\n", i); 
     break;
case EQ_OP:
     printf("%04d:  EQ_OP\n", i); 
     break;
case NE_OP:
     printf("%04d:  NE_OP\n", i); 
     break;
case STOP :
     printf("%04d:  STOP\n", i); 
     break;
case STOR:
     if(mem[k][i+1]<230)
     { searchsym(name, mem[k][i+1]);
       printf("%04d:  STOR\t %d  (%s)\n", i, mem[k][i+1], name);
     }
     else
       printf("%04d:  STOR\t %d\n", i, mem[k][i+1]); 
     i++;
     break;
case ST :
     printf("%04d:  ST\n", i); 
     break;
case LOCK :
     printf("%04d:  LOCK\n", i); 
     break;
case UNLOCK :
     printf("%04d:  UNLOCK\n", i); 
     break;
case HALT :
     printf("%04d:  HALT\n", i); 
     break;
case JFALSE :
     printf("%04d:  JFALSE\t %d\n",  i, mem[k][i+1]); i++; 
     break;
case JMP: 
     printf("%04d:  JMP\t %d\n", i, mem[k][i+1]); i++; 
     break;
default:
     printf("(%04d:   %d)\n", i, mem[k][i]);  
     break;
     }
  }
} // end of printing each proc memory

}











