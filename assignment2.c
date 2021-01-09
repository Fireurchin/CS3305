/* Mingpei Dou */
/* 251056543 */
/* mdou */
/* CS3305A Haque */
/* 2020/10/04 */
/* Assignment 2 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Reading and writing end of the pipe
#define READ_END 0
#define WRITE_END 1

int main(int argc, char **argv){
	// instance variables
	int tunnel[2];
	pid_t pid1, j;
	int status, c;
	char cat[16], test2[16];
	
	// If the user does not pass X, Y and Z, the program will terminate
	if (argc != 4){
		printf("Invalid arguments\n");
		exit(0);
	}
	
	// terminate if pipe fails to create
	if ( pipe(tunnel)<0 ){
		perror("Pipe Error");
		exit(0);
	}
	
	printf("A pipe is created for communication between parent (PID %d) and child\n", getpid());
	pid1 = fork();																		// create fork
	if(pid1 < 0){																		// if fork() fails then terminate
		printf("child failed to create");
		exit(1);
	}
	
	if( pid1 > 0 ){																		// if parent
		j = getpid();
		printf("parent (PID %d) created a child (PID %d)\n", j, pid1);
		printf("parent (PID %d) receives X = \"%s\" from the user\n", j, argv[1]);
		wait(NULL);																		// wait for child to finish execution
		read(tunnel[0],&test2,16);														// perform read operation on string Y'
		printf("parent (PID %d) reads Y' from the pipe (Y' = \"%s\")\n", j, test2);
		strcpy(cat,argv[1]);															// copy string X to array
		strcat(cat,test2);																// finish array by concatenating X to Y'
		printf("parent (PID %d) concatenates X and Y' to generate the string: %s\n", j, cat);
	}
	
	if( pid1 == 0 ){																	// if child
		j = getpid();
		printf("child (PID %d) receives Y = \"%s\" and Z = \"%s\" from the user\n", j, argv[2], argv[3]);
		strcpy(test2,argv[2]);															// copy string Y to string Y'
		strcat(test2,argv[3]);															// concatenate Y and X to Y'
		printf("child (PID %d) concatenates Y and Z to generate Y'= %s\n", j, test2);
		write(tunnel[1],test2,16);														// write string Y' to pipe
		printf("child (PID %d) writes Y' into the pipe\n", j);
	}
	
	return 0;																			// return when complete
}