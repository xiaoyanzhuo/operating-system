/*
 *  Created on: Oct 17, 2017
 *  Author: Xiaoyan Zhuo
 *  Part2.c 
 *  For OS Assignment2. Any number of provider and buyer can be used in this program
    to observe the mutex and semaphore work for synchronization.
    Modify the semaphore for provider in Part2 since there is only one provider in Part2.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
  
 
 
#define SIZE 10
#define BUYER_LOOPS 2
  
#define TRUE 1
#define FALSE 0
 
typedef int buffer_t;
buffer_t buffer[SIZE];
int buffer_index;
int counter = 0; 
int buyerThreads_global;
int providerThreads_global;
int *provider_counter;
  
pthread_mutex_t buffer_mutex;
sem_t sem; //we just need use one semaphore in Part2

//check overflow and underflow
void insertbuffer(buffer_t value) {
    if (buffer_index < SIZE) {
        buffer[buffer_index++] = value;
    } else {
        printf("Buffer overflow\n");
    }
}
  
buffer_t dequeuebuffer() {
    if (buffer_index > 0) {
        return buffer[--buffer_index]; // decrease buffer index
    } else {
        printf("Buffer underflow\n");
    }
    return 0;
}
  
//for making spurios wakeups check  
int isempty() {           
    if (buffer_index == 0)
        return TRUE;
    return FALSE;  
}
  
int isfull() {
    if (buffer_index == SIZE) 
        return TRUE;
    return FALSE;
}

/* one provider thread in Part2. 
   so it only requires mutex to lock and unlock while inserting items and doesn't require semaphore.*/
void *provider(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    buffer_t value;
    printf("Create Provider thread %d and start to insert number\n", thread_numb);
    while(counter < buyerThreads_global*2){   //we let each buyer buy twice, and then provider can stop.
        sleep(rand() % 3);
        value = rand() % 5;
        //Change most of codes here, remove semaphore for provider.
        pthread_mutex_lock(&buffer_mutex);
        insertbuffer(value);
        provider_counter[thread_numb]++;
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&sem); // post (increment) buffer semaphore to inform that available items can be removed.
        printf("Provider %d added %d to buffer\n", thread_numb, value);
    }
    pthread_exit(0);
}

/* six buyer threads in Part2.
   keep the same usuage of semaphore and mutex as they work in Part1. */
void *buyer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    buffer_t value;
    int i=0;
    while (i++ < BUYER_LOOPS) {
        pthread_mutex_lock(&buffer_mutex);
        do {
            pthread_mutex_unlock(&buffer_mutex);
            sem_wait(&sem);
            pthread_mutex_lock(&buffer_mutex);
        } while (isempty()); //check for spurios wakeups
        value = dequeuebuffer(value);
        counter++;
        //sem_post(&empty_sem); 
        pthread_mutex_unlock(&buffer_mutex);
        if (i == 1)
            printf("Buyer %d dequeue %d from buffer\n", thread_numb, value);
        if (i == 2)
            printf("Buyer %d second time dequeue %d from buffer\n", thread_numb, value); 
            //for showing each buyer has bought at least one time, second time here.
   }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    buffer_index = 0;
    int providerThreads, buyerThreads, totalThreads;
    if(argc != 3) 
    {
    fprintf(stderr, "Useage: <provider threads> <buyer threads>\n");
    return -1; 
    }
    providerThreads = atoi(argv[1]);
    buyerThreads = atoi(argv[2]);
    buyerThreads_global = atoi(argv[2]);    //global variable, for checking the termination conditions in provider()
    totalThreads = providerThreads + buyerThreads;
    provider_counter = malloc( providerThreads * sizeof(int));  //create provider_counter array, using pointer and malloc.
    int m;
    for (m = 0; m< providerThreads; m++){     
        provider_counter[m] = 0;            //initialize each element of provider_counter as 0.
    }

    int state1, state2;
    state1 = pthread_mutex_init(&buffer_mutex, NULL);
    state2 = sem_init(&sem, 0 ,0);
    //only need to init one semaphore; keep these for comparing purpose
    // state3 = sem_init(&empty_sem, 0, SIZE);

    // if(state1 || state2 || state3 != 0)       //check initialization of mutex and sem.
    if(state1 || state2 != 0)                    //In Mac OS, sem_init will return -1, but in Ubuntu or Fedra will return 0.
        puts("Error mutex & semaphore initialization!!!");  

    pthread_t thread[totalThreads];
    int thread_numb[totalThreads];              //For showing thread from 0 to totalthreads, more clear to check the results.

    //generate provider and buyer threads
    int i, j, k;
    for (i = 0; i < providerThreads; i++){
        thread_numb[i] = i;
        pthread_create(&thread[i],NULL,provider,&thread_numb[i]); 
    }
    printf("Create Buyer threads (%d ~ %d) and ready to remove number\n", providerThreads, totalThreads-1);
    for(j = providerThreads; j < totalThreads; j++) {
        thread_numb[j] = j;
        pthread_create(&thread[j], // pthread_t *t
                       NULL, // const pthread_attr_t *attr
                       buyer, // void *(*start_routine) (void *)
                       &thread_numb[j]);  // void *arg
    }

    for (k = 0; k < totalThreads; k++){
        pthread_join(thread[k], NULL);
    }
    printf("Terminate ------ \n");
    printf("Total transaction times: %d\n", counter);
    int n;
    for (n = 0; n < providerThreads; n++){
        printf("Provider % d provides %d numbers\n", n, provider_counter[n]);  //print out each provider provide how many numbers.
    }
  
    pthread_mutex_destroy(&buffer_mutex);
    sem_destroy(&sem);
    // we use only one semaphore for buyer
    // sem_destroy(&full_sem);
    // sem_destroy(&empty_sem); 
  
    return 0;
    free(provider_counter); 
}







