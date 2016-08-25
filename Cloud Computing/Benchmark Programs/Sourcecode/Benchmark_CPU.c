///////////////////////////////////////////////////////////////////////
///////                                                        ////////
///////                CPU BENCHMARK PROGRAM                   ////////
///////           AUTHOR : PRASANTH BHAGAVATULA                ////////
///////           CWID   : A20355611                           ////////
///////           COURSE : CS553 - CLOUD COMPUTING             ////////
///////       ASSIGNMENT : ASSIGNMENT-1                        ////////
///////           MAIL-ID: PBHAGAV1@HAWK.IIT.EDU               ////////
///////                                                        ////////
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>
#include <math.h>

#define INP_SIZE 400

// Structure for the Integer Matrices
typedef struct benchmark_cpu_matrix_int{
    int a[INP_SIZE][INP_SIZE];
    int b[INP_SIZE][INP_SIZE];
    int result[INP_SIZE][INP_SIZE];
} benchmark_cpu_matrix_int;

// Structure for the Float Matrices
typedef struct benchmark_cpu_matrix_float{
    float a[INP_SIZE][INP_SIZE];
    float b[INP_SIZE][INP_SIZE];
    float result[INP_SIZE][INP_SIZE];
} benchmark_cpu_matrix_float;

// Structure for the threads
typedef struct benchmark_cpu_thread{
    pthread_t threadId;
    int threadNum;
    int start_rowId;
    int end_rowId;
    double start_proc, end_proc;
    double opsPerSec;
    benchmark_cpu_matrix_int *matrices_int;
    benchmark_cpu_matrix_float *matrices_float;
} benchmark_cpu_thread;

void benchmark_cpu();
void benchmark_int(int numOfThreads);
void benchmark_float(int numOfThreads);
void benchmark_10min_int();
void benchmark_10min_float();
void generate_matrix_int(int matrix[INP_SIZE][INP_SIZE]);
void *multiply_matrix_int(void *args);
void generate_matrix_float(float matrix[INP_SIZE][INP_SIZE]);
void *multiply_matrix_float(void *args);

int main(){
    benchmark_cpu();
    return 0;
}

// Function to call the different benchmark experiments
void benchmark_cpu(){
    benchmark_int(1);
    benchmark_int(2);
    benchmark_int(4);
    printf("\n");
    benchmark_float(1);
    benchmark_float(2);
    benchmark_float(4);
    /*benchmark_10min_int();
    benchmark_10min_float();*/
}


// Function to measure the GIOPS
void benchmark_int(int numOfThreads){
    benchmark_cpu_matrix_int *matrices_int = malloc(sizeof(benchmark_cpu_matrix_int));
    double iops, giops, numOfOps, numOfOpsperLoop;
    double thread_run_time;
    struct timeval start, end;
    double start_secs, end_secs, total_secs;
    int i, j, k, sizeOfThreadInp;
    generate_matrix_int(matrices_int->a);
    generate_matrix_int(matrices_int->b);
    benchmark_cpu_thread *threads = malloc(sizeof(benchmark_cpu_thread) * numOfThreads);
    sizeOfThreadInp = INP_SIZE / numOfThreads;
    
    // Initialise thread details
    for (i=0; i<numOfThreads; i++){
        threads[i].threadNum = i;
        threads[i].start_rowId = i * sizeOfThreadInp;
        threads[i].end_rowId = threads[i].start_rowId + sizeOfThreadInp - 1;
        threads[i].matrices_int = matrices_int;
    }
    
    // Get the start time
    gettimeofday(&start, NULL);
    for (i=0; i<numOfThreads; i++){
        pthread_create(&threads[i].threadId, NULL, &multiply_matrix_int, &threads[i]);
    }
    
    for (j=0; j<numOfThreads; j++){
        pthread_join(threads[j].threadId, NULL);
    }
    
    // Get the end time
    gettimeofday(&end, NULL);
    start_secs = (double) (start.tv_sec) + ((double) (start.tv_usec) / 1.0e6);
    end_secs = (double) (end.tv_sec) + ((double) (end.tv_usec) / 1.0e6);
    total_secs = ((double) (end.tv_sec - start.tv_sec)) + ((double) (end.tv_usec - start.tv_usec) / 1.0e6);    

    // Computing the total number of Operations per thread
    numOfOpsperLoop = (((2 * (double) sizeOfThreadInp)) + ((2 * (double) (sizeOfThreadInp) * (double) (INP_SIZE))) + ((6 * (double) (sizeOfThreadInp) * (double) (INP_SIZE) * (double) (INP_SIZE))));

    // Calculating IOPS for each thread
    iops = 0.0;
    for (i=0;i<numOfThreads;i++){
        threads[i].opsPerSec = numOfOpsperLoop / ((double) (threads[i].end_proc - threads[i].start_proc));
        iops += threads[i].opsPerSec;
    }
 
    numOfOps = (double) (numOfThreads) * numOfOpsperLoop;
    
    // Converting IOPS to GIOPS
    giops = iops / 1.0e9;
    
    printf("CPU Benchmark for Integer Datatype for %d threads is %lf GIOPS\n", numOfThreads, giops);
    /*printf("\nOther Inputs: Number of Ops = %lf and time taken: %lf", numOfOps, total_secs);
    printf("\nStart secs: %lf, End secs: %lf", start_secs, end_secs);
    printf("\n\nThread Details:");
    for (i=0; i<numOfThreads; i++){
        thread_run_time = (threads[i].end_proc - threads[i].start_proc);
        printf("\nThread num: %d, Start Row: %d, End Row: %d", threads[i].threadNum, threads[i].start_rowId, threads[i].end_rowId);
        printf("\nRun time: %lf, Start time: %lf, End time: %lf", thread_run_time, threads[i].start_proc, threads[i].end_proc);
    }
    printf("\n\n");*/
    
    free(matrices_int);
    free(threads);
}

// Function to generate the INT Input Matrix
void generate_matrix_int(int matrix[INP_SIZE][INP_SIZE]){
    int row, col;
    time_t t;
    srand((unsigned) time(&t));
    for (row = 0; row < INP_SIZE; row++){
        for (col = 0; col < INP_SIZE; col++){
            matrix[row][col] = rand();
        }
    }
}

// Function to multiply the INT Input Matrices
void *multiply_matrix_int(void *args){
    benchmark_cpu_thread *thread = args;
    benchmark_cpu_matrix_int *matrices_int = thread->matrices_int;
    int row, col, index;
    struct timeval thread_start, thread_end;

    // Get the start time
    gettimeofday(&thread_start, NULL);
    for (row = thread->start_rowId; row <= thread->end_rowId; row++){
        for (col = 0; col < INP_SIZE; col++){
            matrices_int->result[row][col] = 0;
            for (index = 0; index < INP_SIZE; index++){
                matrices_int->result[row][col] += ((matrices_int->a[row][index] * 25) * (matrices_int->b[index][col] / 25));
            }
        }
    }

    // Get the end time
    gettimeofday(&thread_end, NULL);
    
    // Store the start and end time in thread structure
    thread->start_proc = (double) (thread_start.tv_sec) + ((double) (thread_start.tv_usec) / pow(10, 6));
    thread->end_proc = (double) (thread_end.tv_sec) + ((double) (thread_end.tv_usec) / pow(10, 6));
}

// Function to measure the GFLOPS
void benchmark_float(int numOfThreads){
    benchmark_cpu_matrix_float *matrices_float = malloc(sizeof(benchmark_cpu_matrix_float));
    double flops, gflops, numOfOps, numOfOpsperLoop;
    double thread_run_time;
    struct timeval start, end;
    double start_secs, end_secs, total_secs;
    int i, j, sizeOfThreadInp;
    generate_matrix_float(matrices_float->a);
    generate_matrix_float(matrices_float->b);
    benchmark_cpu_thread *threads = malloc(sizeof(benchmark_cpu_thread) * numOfThreads);
    sizeOfThreadInp = INP_SIZE / numOfThreads;
    
    // Initialize the thread details
    for (i=0; i<numOfThreads; i++){
        threads[i].threadNum = i;
        threads[i].start_rowId = i * sizeOfThreadInp;
        threads[i].end_rowId = threads[i].start_rowId + sizeOfThreadInp - 1;
        threads[i].matrices_float = matrices_float;
    }
    
    // Get the start time
    gettimeofday(&start, NULL);
    for (i=0; i<numOfThreads; i++){
        pthread_create(&threads[i].threadId, NULL, &multiply_matrix_float, &threads[i]);
    }
    
    for (j=0; j<numOfThreads; j++){
        pthread_join(threads[j].threadId, NULL);
    }
    // Get the end time
    gettimeofday(&end, NULL);

    // Compute the total tiem taken
    start_secs = (double) (start.tv_sec) + ((double) (start.tv_usec) / pow(10, 6));
    end_secs = (double) (end.tv_sec) + ((double) (end.tv_usec) / pow(10, 6));
    total_secs = ((double) (end.tv_sec - start.tv_sec)) + ((double) (end.tv_usec - start.tv_usec) / pow(10, 6));

    // Computing the total number of Operations per thread
    numOfOpsperLoop = (((2 * (double) sizeOfThreadInp)) + ((2 * (double) (sizeOfThreadInp) * (double) (INP_SIZE))) + ((6 * (double) (sizeOfThreadInp) * (double) (INP_SIZE) * (double) (INP_SIZE))));

    // Calculating the Operations per second for each thread
    flops = 0.0;
    for (i=0;i<numOfThreads;i++){
        threads[i].opsPerSec = numOfOpsperLoop / ((double) (threads[i].end_proc - threads[i].start_proc));
        flops += threads[i].opsPerSec;
    }

    numOfOps = (double) (numOfThreads) * numOfOpsperLoop;

    // Converting FLOPS to GFLOPS
    gflops = flops / pow(10, 9);
    
    printf("CPU Benchmark for Float Datatype for %d threads is %lf GFLOPS\n", numOfThreads, gflops);
    /*printf("\nOther Inputs: Number of Ops = %lf and time taken: %lf", numOfOps, total_secs);
    printf("\nStart secs: %lf, End secs: %lf", start_secs, end_secs);
    printf("\n\nThread Details:");
    for (i=0; i<numOfThreads; i++){
        thread_run_time = (threads[i].end_proc - threads[i].start_proc);
        printf("\nThread num: %d, Start Row: %d, End Row: %d", threads[i].threadNum, threads[i].start_rowId, threads[i].end_rowId);
        printf("\nRun time: %lf, Start time: %lf, End time: %lf", thread_run_time, threads[i].start_proc, threads[i].end_proc);
    }
    printf("\n\n");*/
    
    free(matrices_float);
    free(threads);
}

// Function to generate the FLOAT Input Matricx
void generate_matrix_float(float matrix[INP_SIZE][INP_SIZE]){
    int row, col;
    time_t t;
    srand((unsigned) time(&t));
    for (row = 0; row < INP_SIZE; row++){
        for (col = 0; col < INP_SIZE; col++){
            matrix[row][col] = (float) rand();
        }
    }
}

// Function to multiply the FLOAT Input Matrices
void *multiply_matrix_float(void *args){
    benchmark_cpu_thread *thread = args;
    benchmark_cpu_matrix_float *matrices_float = thread->matrices_float;
    int row, col, index;
    struct timeval thread_start, thread_end;

    // Get the start time
    gettimeofday(&thread_start, NULL);
    for (row = thread->start_rowId; row <= thread->end_rowId; row++){
        for (col = 0; col < INP_SIZE; col++){
            matrices_float->result[row][col] = 0.0;
            for (index = 0; index < INP_SIZE; index++){
                // Matrix multiplication
                matrices_float->result[row][col] += ((matrices_float->a[row][index] * 25.0) * (matrices_float->b[index][col] / 25.0));
            }
        }
    }
    // Get the end time
    gettimeofday(&thread_end, NULL);

    // Store the start and end time in thread structure 
    thread->start_proc = (double) (thread_start.tv_sec) + ((double) (thread_start.tv_usec) / pow(10, 6));
    thread->end_proc = (double) (thread_end.tv_sec) + ((double) (thread_end.tv_usec) / pow(10, 6));
}
