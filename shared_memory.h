/* Shuvaethy Neill */

#define LIST_SIZE 7
#define NUM_CHILD_PROCESS 3

struct shared_use_st {
	char AR[LIST_SIZE]; 
	int firstProcessDone; 
	int secondProcessDone; 
	int thirdProcessDone; 
};

/* semun.h taken from the course Lnux textbook*/

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
