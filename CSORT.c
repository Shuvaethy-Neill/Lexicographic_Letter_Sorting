/* Shuvaethy Neill */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <ctype.h>

#include "shared_memory.h"

//Declaring semaphore functions
int set_semvalue(int sem_id);
void del_semvalue(int sem_id);
int semaphore_p(int sem_id);
int semaphore_v(int sem_id);

//Declaring functions used to sort
void childProcessSort(struct shared_use_st *shared_stuff, int debug, int i, int firstSem, int secondSem);
int swap(int m, int n, struct shared_use_st *shared_stuff);

int main(void) {
    pid_t pid;
    //determines if user chose debug mode (1) or not (0)
    int debug;          
    //id of binary sem associated with AR[2]
    static int firstSem;
    //id of binary sem associated with AR[4]  
    static int secondSem;   
    //array to hold user input list
    char userInput[LIST_SIZE];     

    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    int shmid;
 
    shmid = shmget((key_t)8374, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
  
    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    /*Make the shared memory accessibe to the program*/
    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
            fprintf(stderr, "shmat failed\n");
            exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n", (int)shared_memory);
    shared_stuff = (struct shared_use_st *)shared_memory; 
    
    /*Ask user if they want debug mode*/
    printf("Would you like to enable debug mode? Enter 0 for disable or 1 for enable: ");
    scanf("%d", &debug);
    //Error handle
    if (debug != 0 && debug != 1) {
        fprintf(stderr, "sorry that is not valid\n");
        exit(EXIT_FAILURE);
    }

    /*Get list of letters from user*/
    printf("Please enter 7 letters that are distinct (hit enter after each character): \n");
	for (int i = 0; i < LIST_SIZE; i ++) {
    	scanf(" %c", &userInput[i]); 
        //Convert uppercase to lowercase
        if (userInput[i] >= 65 && userInput[i] <= 90){
			userInput[i] = userInput[i] + 32;
		}
        // //Store lowercase letter in shared mem array
        // shared_stuff->AR[i] = userInput[i];
	}

    /*Check for duplicate letters in list of letters, if not distinct end program*/
    for(int g = 0; g < LIST_SIZE -1; g++) {    
        for(int h = g + 1; h < LIST_SIZE; h++) {    
            if(userInput[h] == userInput[g]){  
                fprintf(stderr, "Sorry no duplicates allowed. Re-execute and try again.\n");   
			    exit(EXIT_FAILURE);
            }
        }    
    }    

    //copy the inputted array into shared memory
	shared_stuff = (struct shared_use_st *) shared_memory;
	memcpy(shared_stuff->AR, userInput, sizeof(userInput));

	/*Create two semaphores (used by child processes)*/
	firstSem = semget((key_t) (2948), 1, 0666 | IPC_CREAT);
	set_semvalue(firstSem);
	secondSem = semget((key_t) (9826), 1, 0666 | IPC_CREAT);
	set_semvalue(secondSem);

    /*Fork 3 child processes*/
    for (int i = 0; i < NUM_CHILD_PROCESS; i++){
        pid = fork(); 
    
        switch(pid) {

        //invalid
        case -1:
            perror("fork failed");
            exit(1);
        
        //child process
        case 0:
            //Each process sorts by calling sorting function
            childProcessSort(shared_stuff, debug, i, firstSem, secondSem);
            exit(0);

        default:
            break;   
        }
    }
   
    /*Waiting for child processes, make sure they are terminanted*/
    int numProcesses = NUM_CHILD_PROCESS;
    while(numProcesses > 0){
        wait(NULL); 
        numProcesses--;
    }

    /*Print sorted list in comma format*/
    printf("\nSorted list in lexicographic order: ");
    for (int k = 0; k < LIST_SIZE; k++){
       if (k < (LIST_SIZE - 1)){
            printf("%c,",shared_stuff->AR[k]);
       }
       else{
           printf("%c\n",shared_stuff->AR[k]);
       } 
   }
   
   /*Detach and delete shared memory*/
   if (shmdt(shared_memory) == -1) {
       fprintf(stderr,"shmdt failed\n");
       exit(EXIT_FAILURE);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    /*Delete semaphores*/
    del_semvalue(firstSem);
	del_semvalue(secondSem);

    exit(EXIT_SUCCESS);
    return 0;
}

/* Child process calculations for sorting the list of letters*/
void childProcessSort(struct shared_use_st *shared_stuff,int debug, int i, int firstSem, int secondSem){
    //while the flags have not been set (means that something has been swapped and at last one process is not done sorting)
    while(!(shared_stuff->firstProcessDone && shared_stuff->secondProcessDone && shared_stuff->thirdProcessDone)) {
		//reset the flags and check if the swap affected prevous swaps
        shared_stuff->firstProcessDone, shared_stuff->secondProcessDone, shared_stuff->thirdProcessDone = 0;

        /*Process 1 sort responsible for AR[0], AR[1], AR[2]*/
		if(i == 0){
            //variable to keep track of if process has swapped (0 no, 1 yes)
			int swapped = 0;

            //compare ASCII code of letters beside each other
			if(shared_stuff->AR[0] > shared_stuff->AR[1]) { 
                //call swap function
                swapped = swap(0, 1, shared_stuff); 
				//print debug statement if user chose debug mode
                if(debug) printf("Process P%d: performed swapping\n", i + 1);
			}
            //obtain semaphore for AR[2]
			if (!semaphore_p(firstSem)) exit(EXIT_FAILURE); 
			if (shared_stuff->AR[1] > shared_stuff->AR[2]) {
                swapped = swap(1, 2, shared_stuff);	
				if(debug) printf("Process P%d: performed swapping\n", i + 1);
			}
            //release semaphore for AR[2]
			if (!semaphore_v(firstSem)) exit(EXIT_FAILURE); 

			if(!swapped) {
				if(debug) printf("Process P%d: No swapping\n", i + 1);
                //raise flag if process did not swap
				shared_stuff->firstProcessDone = 1;
			}
		}

        /*Process 2 sort responsible for AR[2], AR[3], AR[4]*/
		else if(i == 1) { 
			int swapped = 0;

			if (!semaphore_p(firstSem)) exit(EXIT_FAILURE);
			if(shared_stuff->AR[2]>shared_stuff->AR[3]){ 
                swapped = swap(2, 3, shared_stuff);
				if(debug) printf("Process P%d: performed swapping\n", i + 1);	
			}
			if (!semaphore_v(firstSem)) exit(EXIT_FAILURE);

			if (!semaphore_p(secondSem)) exit(EXIT_FAILURE);
			if(shared_stuff->AR[3]>shared_stuff->AR[4]) {
                swapped = swap(3, 4, shared_stuff);	
				if(debug) printf("Process P%d: performed swapping\n", i + 1);
			}
			if (!semaphore_v(secondSem)) exit(EXIT_FAILURE);	

			if(!swapped) {
				if(debug) printf("Process P%d: No swapping\n", i + 1);
				shared_stuff->secondProcessDone = 1;
			}
		}
			
        /*Process 3 sort responsible for AR[4], AR[5], AR[6]*/
		else if(i == 2) { 
			int swapped = 0;

			if (!semaphore_p(secondSem)) exit(EXIT_FAILURE);
			if(shared_stuff->AR[4] > shared_stuff->AR[5]) {
                swapped = swap(4, 5, shared_stuff);
				if(debug) printf("Process P%d: performed swapping\n", i + 1);	
			}
			if (!semaphore_v(secondSem)) exit(EXIT_FAILURE);			
			
			if(shared_stuff->AR[5] > shared_stuff->AR[6]) {
                swapped = swap(5, 6, shared_stuff);	
				if(debug) printf("Process P%d: performed swapping\n", i + 1);	
			}
			if(!swapped) {
				if(debug) printf("Process P%d: No swapping\n", i + 1);
				shared_stuff->thirdProcessDone = 1;
			}
		}
	}
}

/*Swap function to swap out of order letters beside one another*/
int swap(int m, int n, struct shared_use_st *shared_stuff) {
	char temp = shared_stuff->AR[m];
	shared_stuff->AR[m] = shared_stuff->AR[n];
	shared_stuff->AR[n] = temp;
	return 1;
}

/*Semaphore functions obtained from the Linux course textbook*/

/* The function set_semvalue initializes the semaphore using the SETVAL command in a
 semctl call. We need to do this before we can use the semaphore. */ 
int set_semvalue(int sem_id)
{
    union semun sem_union;

    sem_union.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);
    return(1);
}

void del_semvalue(int sem_id)
{
    union semun sem_union;

    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

/* semaphore_p changes the semaphore by -1 (waiting). */
int semaphore_p(int sem_id)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return(0);
    }
    return(1);
}

/* semaphore_v is similar except for setting the sem_op part of the sembuf structure to 1,
 so that the semaphore becomes available. */
int semaphore_v(int sem_id)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        return(0);
    }
    return(1);
}
