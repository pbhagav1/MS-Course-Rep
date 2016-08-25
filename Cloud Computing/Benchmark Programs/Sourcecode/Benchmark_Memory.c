///////////////////////////////////////////////////////////////////////
///////                                                        ////////
///////             MEMORY BENCHMARK PROGRAM                   ////////
///////           AUTHOR : PRASANTH BHAGAVATULA                ////////
///////           CWID   : A20355611                           ////////
///////           COURSE : CS553 - CLOUD COMPUTING             ////////
///////       ASSIGNMENT : ASSIGNMENT-1                        ////////
///////           MAIL-ID: PBHAGAV1@HAWK.IIT.EDU               ////////
///////                                                        ////////
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>
#include <math.h>

#define NUMOFTRIALS 400

// Structure for thread information
typedef struct benchmark_mem_thread{
    pthread_t threadId;
    int threadNum, sizeOfMem, numOfInpStrings;
    char **src, **dest;
    double start_proc, end_proc, mem_thruput, mem_latency;
} benchmark_mem_thread;

void benchmark_memory();
void benchmark_mem(int size, int numOfThreads);
void benchmark_mem_seq(int size, int numOfThreads);
void benchmark_mem_rnd(int size, int numOfThreads);
void *memory_seq(void *args);
void *memory_rnd(void *args);

int main(){
    benchmark_memory();
    return 0;
}

// Function to call different benchmark experiments
void benchmark_memory(){
    benchmark_mem(1, 1);
    benchmark_mem(1, 2);
    benchmark_mem(1024, 1);
    benchmark_mem(1024, 2);
    benchmark_mem(1048576, 1);
    benchmark_mem(1048576, 2);
}

// Function to call Sequential and Random Access for a particular set of input
void benchmark_mem(int size, int numOfThreads){
    benchmark_mem_seq(size, numOfThreads);
    benchmark_mem_rnd(size, numOfThreads);
}

// Function to determine Throughput and Latency for Sequential access
void benchmark_mem_seq(int size, int numOfThreads){
    int i, j, k, pos;
    double avg_mem_thruput, avg_mem_latency;
    char mem_size[10];
    benchmark_mem_thread *threads = malloc (sizeof(benchmark_mem_thread) * numOfThreads);
    
    // Initialise thread information
    for (i=0; i<numOfThreads; i++){
        threads[i].threadNum = i;
        threads[i].sizeOfMem = size;
        threads[i].numOfInpStrings = (NUMOFTRIALS / numOfThreads);
        threads[i].src  = (char **) malloc((threads[i].numOfInpStrings) * sizeof(char *));
        threads[i].dest = (char **) malloc((threads[i].numOfInpStrings) * sizeof(char *));

        // Create the strings
        for (j=0; j<threads[i].numOfInpStrings; j++){
            threads[i].src[j]  = (char *) malloc(size);
            threads[i].dest[j] = (char *) malloc(size);
            memset(threads[i].src[j], 'a', size);
        }
    }
    
    for (i=0; i<numOfThreads; i++){
        pthread_create(&threads[i].threadId, NULL, &memory_seq, &threads[i]);
    }
    
    for (j=0; j<numOfThreads; j++){
        pthread_join(threads[j].threadId, NULL);
    }
    
    // Calculate the average Throughput and Latency across the threads
    avg_mem_thruput = 0.0;
    avg_mem_latency = 0.0;
    for (i=0; i<numOfThreads; i++){
        avg_mem_thruput += threads[i].mem_thruput;
        avg_mem_latency += threads[i].mem_latency; 
    }
    
    avg_mem_thruput = avg_mem_thruput / numOfThreads;
    avg_mem_latency = avg_mem_latency / numOfThreads;
    
    switch(size){
        case 1:
            strcpy(mem_size, "1 Byte");
            break;
        case 1024:
            strcpy(mem_size, "1 KB");
            break;
        case 1048576:
            strcpy(mem_size, "1 MB");
            break;
    }
    printf("\nMemory benchmark for Sequential Access of %s memory across %d threads is: ", mem_size, numOfThreads);
    printf("\n\tMemory Throughput : %lf MB/sec", avg_mem_thruput);
    printf("\n\tMemory latency    : %lf ms\n", avg_mem_latency);
    
    free(threads);
}

// Function to perform the Sequential Access of memory
void *memory_seq(void *args){
    benchmark_mem_thread *thread = args;
    int i;
    struct timeval start, end;
    clock_t start_tick, end_tick;
    double total_proc_time, total_Mbytes_copied, numOfticks;
    
    // Get the start time and ticks
    gettimeofday(&start, NULL);
    start_tick = clock();
    
    for (i=0; i<thread->numOfInpStrings; i++){
        // Function to sequentially access the memory
        memcpy(thread->dest[i], thread->src[i], thread->sizeOfMem);
    }
    
    // Get the end time and ticks
    end_tick = clock();
    gettimeofday(&end, NULL);

    thread->mem_latency = 0.0;
    thread->mem_thruput = 0.0;
    
    // Calculating Memory Latency for this thread
    numOfticks = (((double) (end_tick - start_tick)) / ((double) (thread->numOfInpStrings)));
    thread->mem_latency = ((numOfticks * 1000) / ((double) (CLOCKS_PER_SEC)));

    // Calculating Memory Throughput for this thread
    total_Mbytes_copied = ((double) (thread->numOfInpStrings * thread->sizeOfMem)) / ((double) (1024 * 1024));
    total_proc_time = ((double) (end.tv_sec - start.tv_sec)) + ((double) (end.tv_usec - start.tv_usec) / 1.0E6);
    thread->mem_thruput = (total_Mbytes_copied) / ((double) (total_proc_time));

    for (i=0;i<thread->numOfInpStrings; i++){
        free(thread->src[i]);
        free(thread->dest[i]);
    }
    free(thread->src);
    free(thread->dest);     
}

// Function to determine Throughput and Latency for Random access
void benchmark_mem_rnd(int size, int numOfThreads){
    int i, j, k, pos;
    double avg_mem_thruput, avg_mem_latency;
    char mem_size[10];
    benchmark_mem_thread *threads = malloc (sizeof(benchmark_mem_thread) * numOfThreads);    
    
    // Initialise thread information
    for (i=0; i<numOfThreads; i++){
        threads[i].threadNum = i;
        threads[i].sizeOfMem = size;
        threads[i].numOfInpStrings = (NUMOFTRIALS / numOfThreads);
        threads[i].src  = (char **) malloc(threads[i].numOfInpStrings * sizeof(char *));
        threads[i].dest = (char **) malloc(threads[i].numOfInpStrings * sizeof(char *));
        for (j=0; j<threads[i].numOfInpStrings; j++){
            // Create the required strings
            threads[i].src[j]  = (char *) malloc(size);
            threads[i].dest[j] = (char *) malloc(size);
            memset(threads[i].src[j], 'a', size);
        }
    }
    
    for (i=0; i<numOfThreads; i++){
        pthread_create(&threads[i].threadId, NULL, &memory_rnd, &threads[i]);
    }
    
    for (j=0; j<numOfThreads; j++){
        pthread_join(threads[j].threadId, NULL);
    }
    
    // Calculating average Throughput and Latency across threads
    avg_mem_thruput = 0.0;
    avg_mem_latency = 0.0;
    for (i=0; i<numOfThreads; i++){
        avg_mem_thruput += threads[i].mem_thruput;
        avg_mem_latency += threads[i].mem_latency; 
    }
    
    avg_mem_thruput = avg_mem_thruput / numOfThreads;
    avg_mem_latency = avg_mem_latency / numOfThreads;
    
    switch(size){
        case 1:
            strcpy(mem_size, "1 Byte");
            break;
        case 1024:
            strcpy(mem_size, "1 KB");
            break;
        case 1048576:
            strcpy(mem_size, "1 MB");
            break;
    }
    printf("\nMemory benchmark for Random Access of %s memory across %d threads is: ", mem_size, numOfThreads);
    printf("\n\tMemory Throughput : %lf MB/sec", avg_mem_thruput);
    printf("\n\tMemory latency    : %lf ms\n", avg_mem_latency);
    
    free(threads);
}

// Function to perform the Random Access of memory
void *memory_rnd(void *args){
    benchmark_mem_thread *thread = args;
    int i, j;
    struct timeval start, end;
    clock_t start_tick, end_tick, total_proc_ticks;
    double total_proc_time, total_Mbytes_copied, total_ticks;
    srand(time(NULL));

    // Get the start time and ticks
    gettimeofday(&start, NULL);
    start_tick = clock();

    for (i=0; i<thread->numOfInpStrings; i++){
        j = (int) (rand() % (thread->numOfInpStrings));
        // Function to randomly access the memory
        memcpy(thread->dest[j], thread->src[j], thread->sizeOfMem);
    }

    // Get the end time and ticks
    end_tick = clock();
    gettimeofday(&end, NULL);
    
    thread->mem_latency = 0.0;
    thread->mem_thruput = 0.0;

    // Calculate the Memory Latency for this thread
    total_proc_ticks = end_tick - start_tick;
    total_ticks = ((double) (total_proc_ticks)) / ((double) (thread->numOfInpStrings));
    thread->mem_latency = ((total_ticks * 1000) / ((double) (CLOCKS_PER_SEC)));
 
    // Calculate the Memory Throughput for this thread
    total_Mbytes_copied = ((double) (thread->numOfInpStrings * thread->sizeOfMem)) / ((double) (1024 * 1024));
    total_proc_time = ((double) (end.tv_sec - start.tv_sec)) + ((double) (end.tv_usec - start.tv_usec) / 1.0E6);
    thread->mem_thruput = (total_Mbytes_copied) / ((total_proc_time));
    
    for (i=0;i<thread->numOfInpStrings; i++){
        free(thread->src[i]);
        free(thread->dest[i]);
    }
    free(thread->src);
    free(thread->dest);     
}
