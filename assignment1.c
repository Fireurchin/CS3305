
/* Mingpei Dou */
/* 251056543 */
/* mdou */
/* Assignment 1 */
/* CS 3305A */
/* 23/09/2020 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv){
	if (argc!=2){										// terminates if path external_program.out isn't provided by the user
		printf("Invalid Arguments\n");
		exit(0);
	}
	char k = 0;
	char strpid2[6];
	pid_t pid1, pid2, i, j;									// declare initial variables
	pid1 = fork();										// create child1
	if(pid1 < 0){										// if fork() fails then terminate
		printf("child failed to create");
		exit(1);
	}
	if (pid1 > 0) wait(NULL);								// if parent then wait for child1
	if (pid1 == 0){										// if child1 then print PID and PPID then wait for child1_1
		i = getpid();
		j = getppid();
		printf("\nparent process (PID %d) created child_1 (PID %d)\nparent (PID %d) is waiting for child_1 (PID %d) to complete before creating child_2\n", j, i, j, i);
		k++;										// initializes testing variable
		wait(NULL);
	}
	pid2 = fork();										// create second successive process
	if (pid2 > 0) wait(NULL);								// if not finished child1 then wait
	if (pid2 < 0){										// if fork() fails then terminate
		printf("child failed to create");
		exit(1);
	}
	if (pid2 == 0){										// if child process
		i = getpid();
		j = getppid();
		if (k>0){									// test for child1_1
			printf("child_1 (PID %d) has created child_1.1 (PID %d)\nchild_1 (PID %d) is now complete\n", j, i, j);			// print if child1_1
		}
		else{																// print if child2
			sprintf(strpid2, "%d", getpid());
			printf("parent (PID %d) has created child_2 (PID %d)\nchild_2 (PID %d) is calling an external program external_program.out and leaving child_2\n", j, i, i);
			execl("external_program.out","external_program",strpid2,NULL);		// execl PID to external program file
			printf("\n\n");
		}
	}
	//free(strpid2);										// free allocated memory from strpid2
	return 0;																// returns from main
}
