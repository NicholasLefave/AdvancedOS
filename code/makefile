all: shell cpu filemanager disk btree boot.c system shellcommands 
	gcc -g boot.c system.o shellcommands.o cpu.o shell.o filemanager.o disk.o btree.o -o startvm

system: system.h system.c
	gcc -g -c system.c 

fileTester: filemanager btree disk system
	gcc -g filemanagerTester.c filemanager.o btree.o disk.o system.o

shellcommands: shellcommands.h shellcommands.c
	gcc -g -c shellcommands.c	

shell: shell.h shell.c
	gcc -g -c shell.c

cpu: cpu.h cpu.c
	gcc -g -c cpu.c

filemanager: disk btree filemanager.h filemanager.c
	gcc -g -c filemanager.c 

disk: disk.h disk.c
	gcc -g -c disk.c

btree: btree.h btree.c
	gcc -g -c btree.c

clean:
	rm *.o


