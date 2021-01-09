/* Mingpei Dou */
/* 251056543 */
/* mdou */
/* CS3305A Haque */
/* 2020/11/25 */
/* Assignment 5+6 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

// instance variables
int balance, balance1, balance2, balanceGot;
pthread_mutex_t lock;
int balArray[150];

typedef struct client{	// initialize struct client
	char operation;
	char acc1[2];
	char acc2[2];
	int amount;
}client;

client* opArray[150]; // create array of type client pointer

void  *withdraw(client* operation);
void  *add(client* operation);
void  *transfer(client* operation);

void *withdraw(client* operation) {	// withdraw function
	pthread_mutex_lock(&lock);	// ENTRY
	char tempChar = operation->acc1[1];	// CRIT STARTS
	int id = tempChar - '0' - 1;
	if (balArray[id] >= operation->amount){	// if ammount can be withdrawn
		balArray[id] -= operation->amount;	// withdraw from account
		printf("Transaction Successful: $%d withdrawn from %s\n",operation->amount,operation->acc1);
	}
	else
		printf("Request Denied: Insufficient Funds %d\n",balArray[id]);	// CRIT ENDS
	pthread_mutex_unlock(&lock);	// EXIT
}

void *add(client* operation) {	// deposit function
	pthread_mutex_lock(&lock);	// ENTRY
	char tempChar = operation->acc1[1];	// CRIT STARTS
	int id = tempChar - '0' - 1;
	balArray[id] += operation->amount;	// deposit into account
	printf("Transaction Successful: $%d deposited into %s\n",operation->amount,operation->acc1);	// CRIT ENDS
	pthread_mutex_unlock(&lock);	// EXIT
}

void *transfer(client* operation) {	// transfer function
	pthread_mutex_lock(&lock);	// ENTRY
	int temp1 = operation->acc1[1];	// CRIT STARTS
	int temp2 = operation->acc2[1];
	int id1 = temp1 - '0' - 1;
	int id2 = temp2 - '0' - 1;
	if (balArray[id1]>=operation->amount){	// if amount can be transfered
		balArray[id2] += operation->amount;	// transfer from acc1 to acc2
		balArray[id1] -= operation->amount;
		printf("Transaction Successful: $%d transfered from %s to %s\n",operation->amount,operation->acc1,operation->acc2);
	}
	else
		printf("Request Denied: Insufficient Funds %d\n",balArray[id1]);	// CRIT ENDS
	pthread_mutex_unlock(&lock);	// EXIT
}

int main(int argc, char* argv[]) {
	// instance variables
	int i, err_thread, cmpr, count, count1;
	int j = 0, k = 0, numOps = 0;
	char tempArr[150][20];
	char tempChar[20];
	pthread_t threads[150];
	FILE *fp = fopen(argv[1], "r");	// read input file
	FILE *output;	// initialize output file pointer
	
	if (fp == NULL) {	// return error if file cannot be read
		perror(argv[1]);
		exit(1);
	}

	while (fscanf(fp,"%s",tempChar)==1){	// store each word in array
		strcpy(tempArr[k],tempChar);
		k++;
	}

	for (count = 0; count < 150; count++){
		cmpr = strcmp(tempArr[j],"c1");
		if (cmpr==0)	// if out-of-bounds then break
			break;
		else{
			balArray[count] = (int) strtol(tempArr[j+2],NULL,10);	// store account balances in balArray
			j+=3;
		}
	}

	for(count1 = 0; count1<150; count1++){
		if (j==k)	//(tempArr[j]==NULL)
			break;	// if out-of-bounds then break
		else{
			if (tempArr[j][0]=='c')	// if client then skip
				j++;
			opArray[count1] = (struct client*)malloc(sizeof(client));	// Allocate memory for the struct
			opArray[count1]->operation = tempArr[j][0];
			strcpy(opArray[count1]->acc1,tempArr[j+1]);
			if (tempArr[j][0] == 't'){	// if operation is transfer, make room for second account
				strcpy(opArray[count1]->acc2,tempArr[j+2]);
				j++;
			}
			opArray[count1]->amount = (int) strtol(tempArr[j+2],NULL,10);
			j+=3;
		}
	}
	
	if (pthread_mutex_init(&lock, NULL) != 0) {	// if lock fails return error
		printf("mutex init failed\n");
		return 1;
	}
	
	for (i = 0; i< count1; i++) {
		if (opArray[i]->operation=='w'){	// if op is withdraw then create withdraw thread
			err_thread = pthread_create(&threads[i], NULL, (void *)withdraw, opArray[i]);
			if (err_thread != 0)
				printf("Error creating thread %d\n", i);
			else
				numOps++;
		}
		else if (opArray[i]->operation=='t'){	// if op is deposit then create deposit thread
			err_thread = pthread_create(&threads[i], NULL, (void *)transfer, opArray[i]);
			if (err_thread != 0)
				printf("Error creating thread %d\n", i);
			else
				numOps++;
		}
		else if (opArray[i]->operation=='d'){	// if op is transfer then create transfer thread
			err_thread = pthread_create(&threads[i], NULL, (void *)add, opArray[i]);
			if (err_thread != 0)
				printf("Error creating thread %d\n", i);
			else
				numOps++;
		}
	}
	
	for (i = 0; i< numOps; i++)	// do not terminate threads until all complete
		pthread_join(threads[i], NULL);
		
	pthread_mutex_destroy(&lock);	// destroy lock
	
	for(int k1 = 0; k1 < count1; k1++){	// free client memory
		if (opArray[k1]==NULL)	// if out-of-bounds then break
			break;
		free(opArray[k1]);
	}
	
	output = fopen ("a6_out", "w");	// create file
	char numArr[1]; 
	char balStore[30];
	for (int iter=0;iter<4;iter++){
		int accNum = iter+1;
		sprintf(numArr,"%d",accNum);
		sprintf(balStore,"%d",balArray[iter]);
		printf("a%s b %s\n",numArr,balStore);
		fputs("a", output);	// write to output file
		fputs(numArr, output);	// write to output file
		fputs(" b ", output);	// write to output file
		fputs(balStore, output);	// write to output file
		fputs("\n", output);	// write to output file
	}
	fclose(output);	// close output file
		
	return 0;
}