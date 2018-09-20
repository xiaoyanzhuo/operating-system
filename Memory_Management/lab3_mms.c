#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>
#include <string.h>
//#include <iostream>


#define MAX_MEM_SIZE 64
// #define MAX_NOOF_THREADS 20
#define MAX_MEM_FOR_THREAD 20
#define UNREACHABLE_NO 99999

typedef struct ThreadInfo            //stucture for threads
{
	int pid;
	int size;
}thread;
typedef struct AllocatedBlock                //structure for memory block
{
	char *blkstartAddr;
	int blkSize;
	int memOccupied;
}allocate;

typedef struct AllocatResults
{
    char *return_addr;
    int return_size;
}result;

int FitType,NoOfUThreads,NoOfBlocks,ThreadIndex, totalThreads, Defragement_enable, Less_max, MAX_NOOF_THREADS;
void *GlblMemPtr;                    // Global Variables
char *StartAddr;
char *EndAddr;
// thread Threads[MAX_NOOF_THREADS];
thread *Threads;
allocate Blocks[MAX_MEM_FOR_THREAD];
pthread_mutex_t Mutex;
sem_t sem; 

void InitializeBlocks()   //function to initialize the memory block to all zeroes
{
	char *temp;
	int i;
	temp = StartAddr;                                                  
	for (i = 0;i < MAX_MEM_FOR_THREAD;i++)
	{
		Blocks[i].blkSize = 0;
		Blocks[i].memOccupied = 0;
		Blocks[i].blkstartAddr = StartAddr;
	}
	for (i = 0;i < MAX_MEM_SIZE;i++)
	{
		*temp = 0;
		temp++;
	}
	EndAddr = (temp);
}

void Print_Memory()    //function to print the memory status
{
	int i;
	int j = 1;
	char *index;
	index = StartAddr;
	printf("\n--------MEMORY STATUS---------\n");
	printf("\n\nBlock\tSize\tStart\tEnd\tFlag\tThread\n");
	int block_p, size_p, flag_p, thread_p;
	int start_p, end_p, temp = 0;
	for (i = 0;i < NoOfBlocks;i++){
		start_p = temp;
		size_p = Blocks[i].blkSize;
		end_p = start_p + size_p - 1;
		flag_p = Blocks[i].memOccupied;
		thread_p = *(Blocks[i].blkstartAddr);
		printf("%-5d\t%-5d\t%-5d\t%-5d\t%-5d\t%-5d\n", i+1, size_p, start_p, end_p, flag_p, thread_p);
		temp = start_p + size_p;
	}	


	printf("\nnoofblocks=%d\n\n", NoOfBlocks);
}

void Memory_Info()    //function to update the number of blocks available in the memory, block size, and starting address
{
	NoOfBlocks = 0;
	int i = 1, j, value, blkIndex = 0;
	char *blkStart;
	char *index;
	blkStart = StartAddr;
	index = StartAddr;
	while (index < EndAddr)
	{
		blkStart = index;
		if (*index == 0)
		{
			while (((*index == 0)) && (index < EndAddr))
			{
				index++;
			}
		}
		else if (*index != 0)
		{
			value = *index;
			while ((*index == value) && (index < EndAddr))
			{
				index++;
			}
		}
		Blocks[blkIndex].blkstartAddr = blkStart;
		Blocks[blkIndex].blkSize = index - blkStart;
		if (*blkStart != 0)
			Blocks[blkIndex].memOccupied = 1;
		else
			Blocks[blkIndex].memOccupied = 0;
		blkIndex++;
		NoOfBlocks++;
	}
}

result FirstFit(int blkAllocsize, int threadIndex) 
{
	result ff_result;
	int i, j, k, found = 0,found_1 = 0;
	printf("First Fit\n");
	for (i = 0;i < NoOfBlocks;i++)  // looking for the available block which is greater than the required size
	{
		if ((Blocks[i].memOccupied == 0) && (Blocks[i].blkSize >= blkAllocsize))
		{
			found = 1;
			break;
		}
	}

	if (found == 0)
	{
		printf("NO EMPTY MEMORY AVAILABLE. CHECKING OCCUPIED BLOCKS...\n");
		for (j = 0;j < NoOfBlocks;j++)
		{
			if ((Blocks[j].memOccupied == 1) && (Blocks[j].blkSize >= blkAllocsize))
			{
				found_1 = 1;
				break;
			}
		}
		if (found_1 == 0){
			printf("NO SINGLE OCCUPIED BLOCK AVAILABLE. WAKING MORE BLOCKS TO ALLOCATE\n");
			int sum_size = 0;

			for (k = NoOfBlocks-1;k >= 0;k--){
				sum_size = sum_size + Blocks[k].blkSize;
				if (sum_size >= blkAllocsize)
					break;
			}
			ff_result.return_addr = Blocks[k].blkstartAddr;
			if (Blocks[NoOfBlocks-1].memOccupied == 0)
				ff_result.return_size = blkAllocsize;
			else
				ff_result.return_size = sum_size;
		}
		else{
		ff_result.return_addr = Blocks[j].blkstartAddr;
		ff_result.return_size = Blocks[j].blkSize;
		}

	}
	else{
		ff_result.return_addr = Blocks[i].blkstartAddr;
		if(i < (NoOfBlocks-1))     // not the last blank block
			ff_result.return_size = Blocks[i].blkSize;
		else
			ff_result.return_size = blkAllocsize;
	}
	return ff_result;
}

result BestFit(int blkAllocsize, int threadIndex)
{
	result bf_result;
	int i, j, k, found = 0;	
	int bestID= 0;
	int best_tmp = UNREACHABLE_NO;
	printf("Best Fit\n");
	for (i = 0;i < NoOfBlocks;i++)    //looking for the hole which is greater than the required memory size
	{
		if ((Blocks[i].memOccupied == 0) && (Blocks[i].blkSize >= blkAllocsize) && (Blocks[i].blkSize < best_tmp))
		{
			best_tmp = Blocks[i].blkSize;
			bestID = i;
		}
	}
	if(best_tmp == UNREACHABLE_NO){
		 printf("NO EMPTY MEMORY AVAILABLE. CHECKING OCCUPIED BLOCKS...\n");
		 int best_tmp_1 = UNREACHABLE_NO;
		 for (j = 0;j < NoOfBlocks;j++)
		 {
			if ((Blocks[j].memOccupied == 1) && (Blocks[j].blkSize >= blkAllocsize) && (Blocks[j].blkSize < best_tmp_1))
			{
			best_tmp_1 = Blocks[j].blkSize;
			bestID = j;
			}
		}
		if(best_tmp_1 == UNREACHABLE_NO){
			printf("NO SINGLE OCCUPIED BLOCK AVAILABLE. WAKING MORE BLOCKS TO ALLOCATE\n");
			int sum_size = 0;
			for (k = 0;k < NoOfBlocks;k++){
				sum_size = sum_size + Blocks[k].blkSize;
				if (sum_size >= blkAllocsize)
					break;
			}
			bf_result.return_addr = StartAddr;
			bf_result.return_size = sum_size;
		}
		else{
		bf_result.return_addr = Blocks[bestID].blkstartAddr;
		bf_result.return_size = Blocks[bestID].blkSize;	
		}
	}

	else{
		bf_result.return_addr = Blocks[bestID].blkstartAddr;
		if(bestID < (NoOfBlocks-1))
		{
			bf_result.return_size = Blocks[bestID].blkSize;
		}
		else
			bf_result.return_size = blkAllocsize;
	}

	return bf_result;
}

result WorstFit(int blkAllocsize, int threadIndex)
{
	result wf_result;
	int i, j, k, found = 0;	
	int bestID= 0;
	int best_tmp = 0;
	printf("Worst Fit\n");
	for (i = 0;i < NoOfBlocks;i++)    //looking for the hole which is greater than the required memory size
	{
		if ((Blocks[i].memOccupied == 0) && (Blocks[i].blkSize >= blkAllocsize) && (Blocks[i].blkSize > best_tmp))
		{
			best_tmp = Blocks[i].blkSize;
			bestID = i;
		}
	}
	if(best_tmp == 0){
		 printf("NO EMPTY MEMORY AVAILABLE. CHECKING OCCUPIED BLOCKS...\n");
		 int best_tmp_1 = 0;
		 for (j = 0;j < NoOfBlocks;j++)
		 {
			if ((Blocks[j].memOccupied == 1) && (Blocks[j].blkSize >= blkAllocsize) && (Blocks[j].blkSize > best_tmp_1))
			{
			best_tmp_1 = Blocks[j].blkSize;
			bestID = j;
			}
		}
		if(best_tmp_1 == 0){
			printf("NO SINGLE OCCUPIED BLOCK AVAILABLE. WAKING MORE BLOCKS TO ALLOCATE\n");
			int sum_size = 0;
			for (k = 0;k < NoOfBlocks;k++){
				sum_size = sum_size + Blocks[k].blkSize;
				if (sum_size >= blkAllocsize)
					break;
			}
			wf_result.return_addr = StartAddr;
			wf_result.return_size = sum_size;
		}
		else{
		wf_result.return_addr = Blocks[bestID].blkstartAddr;
		wf_result.return_size = Blocks[bestID].blkSize;	
		}
	}

	else{
		wf_result.return_addr = Blocks[bestID].blkstartAddr;
		if(bestID < (NoOfBlocks-1))
		{
			wf_result.return_size = Blocks[bestID].blkSize;
		}
		else
			wf_result.return_size = blkAllocsize;
	}
	return wf_result;
}

void memory_free(int threadIndex, char *intstartAddr) //Free the allocated memory
{
	int i, j, found = 0;
	char *pointer;
	for (j = 0;j < NoOfBlocks;j++)     //looking for the memory block which is allocated
	{
		if ((intstartAddr == Blocks[j].blkstartAddr) && (*(Blocks[j].blkstartAddr) == threadIndex))
		{
			found = 1;
			break;
		}
	}
	if (found == 0)
	{
		for (j = 0;j < NoOfBlocks;j++)
		{
			if (*(Blocks[j].blkstartAddr) == (threadIndex))
				break;
		}
	}
	intstartAddr = Blocks[j].blkstartAddr;
	for (i = 0;i < Blocks[j].blkSize;i++)
	{
		*intstartAddr = 0;    //freeing the memory
		intstartAddr++;
	}
	Blocks[j].memOccupied = 0;
	Memory_Info();
	printf("\nMEMORY FREE\n");
	Print_Memory();
}

void Memory_Compaction()  //merges the free memory and the allocated memory to make a larger free memory to let the other blocks to be allocated.
{
	int i, j, arrayInd = 0;
	int array[MAX_MEM_SIZE];
	char *pointer, *blockstartAddr, *temp;
	for (i = 0;i < NoOfBlocks;i++)
	{
		if (Blocks[i].memOccupied == 1)
		{
			blockstartAddr = Blocks[i].blkstartAddr;
			for (j = 0;j < Blocks[i].blkSize;j++)
			{
				array[arrayInd] = *blockstartAddr;
				arrayInd++;
				blockstartAddr++;
			}
		}
	}
	temp = StartAddr;
	for (i = 0;i < arrayInd;i++)
	{
		*temp = array[i];
		temp++;
	}
	i = arrayInd;
	while (i < MAX_MEM_SIZE)
	{
		*temp = 0;
		temp++;
		i++;
	}
	temp = StartAddr;
	Memory_Info();
	printf("\nCOMPACTION RESULT\n");
	Print_Memory();
}

void *Memory_Malloc(void *param)    //function called by all the threads
{
	char *pointtoAddr;
	result mms_result;
	int i, j, r, base;
	int blkAlloc;
	Threads[ThreadIndex].pid = (int)pthread_self();
	pthread_mutex_lock(&Mutex);                  //locking the mutex for the thread

	if(Less_max == 1)
		r = rand() % 7;
	else
		r = rand() % 5;
	base = 2;
	blkAlloc = base << r;
	
	if (blkAlloc > MAX_MEM_SIZE)
	{
	printf("Request size:%d, Max size: %d\n", blkAlloc, MAX_MEM_SIZE);
	printf("Request size should be less than available Max size: %d\n", MAX_MEM_SIZE);
	exit(0);
	}

	Threads[ThreadIndex].size = blkAlloc;

	printf("Thread %d: request %d memory space, going to sleep\n", ThreadIndex+1, Threads[ThreadIndex].size);

	printf("MMS: Recieves request of %d memory space from Thread %d\n", Threads[ThreadIndex].size,ThreadIndex+1);

	switch(FitType)
	{
		case 1:
				{
				mms_result = FirstFit(blkAlloc, ThreadIndex);
				break;
				}
			case 2:
				{
				mms_result = BestFit(blkAlloc, ThreadIndex);
				break;
				}
			case 3:
				{
				mms_result = WorstFit(blkAlloc, ThreadIndex);
				break;
				}
	}
	pointtoAddr = mms_result.return_addr;
	for (i = 0; i < mms_result.return_size; i++)
	{
		*pointtoAddr = (ThreadIndex + 1);
		pointtoAddr++;
	}
	Memory_Info();
	printf("\nMEMORY ALLOCATE\n");
	Print_Memory();
	
	pthread_mutex_unlock(&Mutex);                //unlocking the mutex
	ThreadIndex++;
	sleep(rand() % 3);                           //sleeping for random time
	pthread_mutex_lock(&Mutex);                  //locking the mutex 
	if(*mms_result.return_addr != 0){
		printf("Thread %d, waking up\n", *mms_result.return_addr);
		printf("MMS: Recieve dellocation request from Thread %d\n", *mms_result.return_addr);
		memory_free(*mms_result.return_addr, mms_result.return_addr);   //waking up for freeing the memory
	}
	if (Defragement_enable == 1){
		Memory_Compaction();                   //compaction of the memory                               
	}
	pthread_mutex_unlock(&Mutex);                //unlocking the mutex
}

int main(int argc, char *argv[])
{
	int i;
	GlblMemPtr = malloc(MAX_MEM_SIZE);
	StartAddr = (char*)GlblMemPtr;
	InitializeBlocks();         //initializing the memory blocks
	Memory_Info();
	printf("\nMEMORY initialial state\n");
	Print_Memory();

	if(argc != 5){
		fprintf(stderr, "Useage: <threads #N > <fit_type> <Defragement_enable>\n");
		fprintf(stderr, "<fit_type: 1. First_fit, 2. Best_fit, 3. Worst-fit>\n");
		fprintf(stderr, "<Defragement_enable: 0. No Defragement, 1. Defragement>\n");
		fprintf(stderr, "<Request_size large Max_Size: 0. No, 1. Yes>\n");
		return -1; 
    }
    NoOfUThreads = atoi(argv[1]);
    FitType = atoi(argv[2]);
    Defragement_enable = atoi(argv[3]);
    Less_max = atoi(argv[4]);
	Threads = malloc(NoOfUThreads * sizeof(thread));
	
	pthread_t tid[NoOfUThreads];
	for (i = 0;i < NoOfUThreads;i++)  //creating the threads
	{
		pthread_create(&tid[i], NULL, Memory_Malloc, NULL);
	}

	int k;
	for (k = 0; k < NoOfUThreads; k++){
        pthread_join(tid[k], NULL);
    }
    pthread_mutex_destroy(&Mutex);
    sem_destroy(&sem);
	free(GlblMemPtr);
	free(Threads);
	return 0;
}
