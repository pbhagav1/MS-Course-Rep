///////////////////////////////////////////////////////////////////////
///////                                                        ////////
///////               DISK BENCHMARK PROGRAM                   ////////
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

#define NUMOFTRIALS 1200

// Structure for thread details
typedef struct benchmark_dsk_thread{
    pthread_t threadId;
    int threadNum, sizeOfData, numOfIter, startPos;
    char oper, *filename, *src;
    double start_proc, end_proc, dsk_thruput, dsk_latency;
} benchmark_dsk_thread;

void benchmark_disk();
void benchmark_dsk(int size, int numOfThreads);
void benchmark_dsk_seq(int size, int numOfThreads, char oper);
void benchmark_dsk_rnd(int size, int numOfThreads, char oper);
void *disk_seq(void *args);
void *disk_rnd(void *args);

int main(){
    benchmark_disk();
    return 0;
}

// Function to call different benchmark experiments
void benchmark_disk(){
    benchmark_dsk(1, 1);
    benchmark_dsk(1, 2);
    benchmark_dsk(1024, 1);
    benchmark_dsk(1024, 2);
    benchmark_dsk(1048576, 1);
    benchmark_dsk(1048576, 2);
}

// Function to invoke Sequential Write, Sequential Read, Random Write and Random Read
// operations for a particular set of inputs
void benchmark_dsk(int size, int numOfThreads){
    benchmark_dsk_seq(size, numOfThreads, 'w');
    benchmark_dsk_seq(size, numOfThreads, 'r');
    benchmark_dsk_rnd(size, numOfThreads, 'w');
    benchmark_dsk_rnd(size, numOfThreads, 'r');
}


// Function to determine Throughput and Latency for Sequential Access
void benchmark_dsk_seq(int size, int numOfThreads, char oper){
    int i, j, k, pos;
    double avg_dsk_thruput, avg_dsk_latency;
    char dsk_oper[10], dsk_size[10];
    char *filename = malloc(sizeof(char) * 30);
    benchmark_dsk_thread *threads = malloc (sizeof(benchmark_dsk_thread) * numOfThreads);    
    clock_t dummy_clock;

    // Calling dummy clock() function
    dummy_clock = clock();
    
    // Logic to determine the file name
    strcpy(filename, "Benchmark_Disk_Seq_");
    switch(size){
        case 1:
            strcat(filename, "1B.txt");
            break;
        case 1024:
            strcat(filename, "1KB.txt");
            break;
        case 1048576:
            strcat(filename, "1MB.txt");
            break;
    }
    
    // Initialise thread data
    for (i=0; i<numOfThreads; i++){
        threads[i].threadNum = i;
        threads[i].sizeOfData = size;
        threads[i].oper = oper;
        threads[i].numOfIter = (NUMOFTRIALS / numOfThreads);
        threads[i].startPos = i * threads[i].numOfIter;
        threads[i].filename = filename;
        threads[i].src  = (char *) malloc(size);
        memset(threads[i].src, 'a', size);
    }
    
    for (i=0; i<numOfThreads; i++){
        pthread_create(&threads[i].threadId, NULL, &disk_seq, &threads[i]);
    }
    
    for (j=0; j<numOfThreads; j++){
        pthread_join(threads[j].threadId, NULL);
    }
    
    // Calculating average Throughput and Latency across threads
    avg_dsk_thruput = 0.0;
    avg_dsk_latency = 0.0;
    for (i=0; i<numOfThreads; i++){
        avg_dsk_thruput += threads[i].dsk_thruput;
        avg_dsk_latency += threads[i].dsk_latency; 
    }
    
    avg_dsk_thruput = avg_dsk_thruput / numOfThreads;
    avg_dsk_latency = avg_dsk_latency / numOfThreads;
    
    switch(size){
        case 1:
            strcpy(dsk_size, "1 Byte");
            break;
        case 1024:
            strcpy(dsk_size, "1 KB");
            break;
        case 1048576:
            strcpy(dsk_size, "1 MB");
            break;
    }
    
    switch(oper){
        case 'w':
            strcpy(dsk_oper, "Write");
            break;
        case 'r':
            strcpy(dsk_oper, "Read");
            break;
    }
    printf("\nDisk benchmark for Sequential %s operation of %s memory across %d threads is: ", dsk_oper, dsk_size, numOfThreads);
    printf("\n\tDisk Throughput : %lf MB/sec", avg_dsk_thruput);
    printf("\n\tDisk latency    : %lf ms\n", avg_dsk_latency);
    
    free(threads);
}

// Function to Write or Read Data with Sequential Access
void *disk_seq(void *args){
    benchmark_dsk_thread *thread = args;
    int i, freadInd;
    struct timeval start, end;
    clock_t start_tick, end_tick;
    FILE *fptr;
    double total_proc_time, total_Mbytes_copied, numOfticks;
    
    if (thread->oper == 'w'){
        remove(thread->filename);
        fptr = fopen(thread->filename, "w");
        if (!fptr){
            printf("\nFILE OPEN ERROR");
        }else{
            
            // Get the start time and ticks
            gettimeofday(&start, NULL);
            start_tick = clock();
            
            // Positioning file pointer based on the start position of this thread
            fseek(fptr,((thread->startPos) * (thread->sizeOfData)) ,SEEK_SET);
            for (i=0; i<thread->numOfIter; i++){
                fwrite(thread->src,thread->sizeOfData,1,fptr);
            }
            
            // Get the end time and ticks
            end_tick = clock();
            gettimeofday(&end, NULL);
        }    
    }else{
        if (thread->oper == 'r'){
            fptr = fopen(thread->filename, "r");
            if (!fptr){
                printf("\nFILE OPEN ERROR");
            }else{
                
                // Get the start time and ticks
                gettimeofday(&start, NULL);
                start_tick = clock();
                // Positioning file pointer based on the start position of this thread
                fseek(fptr,((thread->startPos) * (thread->sizeOfData)) ,SEEK_SET);
                for (i=0; i<thread->numOfIter; i++){
                    freadInd = fread(thread->src,thread->sizeOfData,1,fptr);
                }
                
                // Get the end time and ticks
                end_tick = clock();
                gettimeofday(&end, NULL);
            }
        }
    }
    
    fclose(fptr);
    
    thread->dsk_latency = 0.0;
    thread->dsk_thruput = 0.0;
    
    //printf("\n\t End ticks: %lf, Start ticks: %lf, Total Ticks: %lf", (double) end_tick, (double) start_tick, (double) (end_tick - start_tick));

    // Calculating Latency
    numOfticks = (((double) (end_tick - start_tick)) / ((double) (thread->numOfIter)));
    thread->dsk_latency = ((numOfticks * 1000) / ((double) (CLOCKS_PER_SEC)));

    // Calculating Throughput
    total_Mbytes_copied = ((double) (thread->numOfIter * thread->sizeOfData)) / ((double) (1024 * 1024));
    total_proc_time = ((double) (end.tv_sec - start.tv_sec)) + ((double) (end.tv_usec - start.tv_usec) / 1.0E6);
    thread->dsk_thruput = (total_Mbytes_copied) / ((double) (total_proc_time));

    free(thread->src);
}

// Function to determine Throughput and Latency for Sequential Access
void benchmark_dsk_rnd(int size, int numOfThreads, char oper){
    int i, j, k, pos;
    double avg_dsk_thruput, avg_dsk_latency;
    char dsk_oper[10], dsk_size[10];
    char *filename = malloc(sizeof(char) * 30);
    benchmark_dsk_thread *threads = malloc (sizeof(benchmark_dsk_thread) * numOfThreads);    
    
    // Logic to determine the file name
    strcpy(filename, "Benchmark_Disk_Rnd_");
    switch(size){
        case 1:
            strcat(filename, "1B.txt");
            break;
        case 1024:
            strcat(filename, "1KB.txt");
            break;
        case 1048576:
            strcat(filename, "1MB.txt");
            break;
    }
    
    // Initialise thread data
    for (i=0; i<numOfThreads; i++){
        threads[i].threadNum = i;
        threads[i].sizeOfData = size;
        threads[i].oper = oper;
        threads[i].numOfIter = (NUMOFTRIALS / numOfThreads);
        threads[i].startPos = i * threads[i].numOfIter;
        threads[i].filename = filename;
        threads[i].src  = (char *) malloc(size);
        memset(threads[i].src, 'a', size);
    }
    
    for (i=0; i<numOfThreads; i++){
        pthread_create(&threads[i].threadId, NULL, &disk_rnd, &threads[i]);
    }
    
    for (j=0; j<numOfThreads; j++){
        pthread_join(threads[j].threadId, NULL);
    }
    
    // Calculating average Throughput and Latency across threads
    avg_dsk_thruput = 0.0;
    avg_dsk_latency = 0.0;
    for (i=0; i<numOfThreads; i++){
        avg_dsk_thruput += threads[i].dsk_thruput;
        avg_dsk_latency += threads[i].dsk_latency; 
    }
    
    avg_dsk_thruput = avg_dsk_thruput / numOfThreads;
    avg_dsk_latency = avg_dsk_latency / numOfThreads;
    
    switch(size){
        case 1:
            strcpy(dsk_size, "1 Byte");
            break;
        case 1024:
            strcpy(dsk_size, "1 KB");
            break;
        case 1048576:
            strcpy(dsk_size, "1 MB");
            break;
    }
    
    switch(oper){
        case 'w':
            strcpy(dsk_oper, "Write");
            break;
        case 'r':
            strcpy(dsk_oper, "Read");
            break;
    }

    printf("\nDisk benchmark for Random %s operation of %s memory across %d threads is: ", dsk_oper, dsk_size, numOfThreads);
    printf("\n\tDisk Throughput : %lf MB/sec", avg_dsk_thruput);
    printf("\n\tDisk latency    : %lf ms\n", avg_dsk_latency);
    
    free(threads);
}

// Function to Write or Read Data with Random Access
void *disk_rnd(void *args){
    benchmark_dsk_thread *thread = args;
    int i, j, freadInd;
    struct timeval start, end, start_overhead, end_overhead;
    clock_t start_tick, end_tick, start_overhead_tick, end_overhead_tick, total_proc_ticks, total_ovr_ticks;
    double total_proc_time, total_ovr_time, total_Mbytes_copied, total_ticks;
    FILE *fptr;

    srand(time(NULL));
    if (thread->oper == 'w'){
        remove(thread->filename);
        fptr = fopen(thread->filename, "w");
        if (!fptr){
            printf("\nFILE OPEN ERROR");
        }else{

            // Get the start time and ticks
            gettimeofday(&start, NULL);
            start_tick = clock();

            for (i=0; i<thread->numOfIter; i++){
                j = (int) (rand() % (thread->numOfIter));
                
                // Positioning file pointer based on the start position of this thread
                fseek(fptr,((thread->startPos + j) * (thread->sizeOfData)),SEEK_SET);
                fwrite(thread->src,thread->sizeOfData,1,fptr);
            }
            
            // Get the end time and ticks
            end_tick = clock();
            gettimeofday(&end, NULL);
        }    
    }else{
        if (thread->oper == 'r'){
            fptr = fopen(thread->filename, "r");
            if (!fptr){
                printf("\nFILE OPEN ERROR");
            }else{
                
                // Get the start time and ticks
                gettimeofday(&start, NULL);
                start_tick = clock();
                
                for (i=0; i<thread->numOfIter; i++){
                    j = (int) (rand() % (thread->numOfIter));
                    
                    // Positioning file pointer based on the start position of this thread
                    fseek(fptr,((thread->startPos + j) * (thread->sizeOfData)),SEEK_SET);
                    freadInd = fread(thread->src,thread->sizeOfData,1,fptr);
                }
                
                // Get the end time and ticks
                end_tick = clock();
                gettimeofday(&end, NULL);
            }
        }
    }
    
    fclose(fptr);
    
    // Get the start time and ticks for Overhead
    gettimeofday(&start_overhead, NULL);
    start_overhead_tick = clock();

    for (i=0; i<thread->numOfIter; i++){
        j = (int) (rand() % (thread->numOfIter));
    }
    
    // Get the end time and ticks for Overhead
    end_overhead_tick = clock();
    gettimeofday(&end_overhead, NULL);
    
    thread->dsk_latency = 0.0;
    thread->dsk_thruput = 0.0;

    //printf("\n\t End ticks: %lf, Start ticks: %lf, Total Ticks: %lf", (double) end_tick, (double) start_tick, (double) (end_tick - start_tick));
    //printf("\n\t End Overhead ticks: %lf, Start Overhead ticks: %lf, Total Ticks: %lf", (double) end_overhead_tick, (double) start_overhead_tick, (double) (end_overhead_tick - start_overhead_tick));


    // Calculating Latency
    total_proc_ticks = end_tick - start_tick;
    total_ovr_ticks  = end_overhead_tick - start_overhead_tick;
    total_ticks = ((double) (total_proc_ticks - total_ovr_ticks)) / ((double) (thread->numOfIter));

    thread->dsk_latency = ((total_ticks * 1000) / ((double) (CLOCKS_PER_SEC)));
 
    // Calculating throughput
    total_Mbytes_copied = ((double) (thread->numOfIter * thread->sizeOfData)) / ((double) (1024 * 1024));
    total_proc_time = ((double) (end.tv_sec - start.tv_sec)) + ((double) (end.tv_usec - start.tv_usec) / 1.0E6);
    total_ovr_time = ((double) (end_overhead.tv_sec - start_overhead.tv_sec)) + ((double) (end_overhead.tv_usec - start_overhead.tv_usec) / 1.0E6);
    thread->dsk_thruput = (total_Mbytes_copied) / ((double) (total_proc_time - total_ovr_time));
 
    free(thread->src);
}
