#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_ITERATIONS 1000000
#define BUFFER_SIZE 10
#define MATRIX_SIZE 500  // Size of the matrices for the multiplication

// Producer-consumer problem with buffer
int buffer[10];
int in = 0, out = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

// Matrix multiplication function (Naive method)
void matrix_multiply(int *A, int *B, int *C, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            C[i * size + j] = 0;
            for (int k = 0; k < size; k++) {
                C[i * size + j] += A[i * size + k] * B[k * size + j];
            }
        }
    }
}

// Function to generate random matrix
void generate_matrix(int *matrix, int size) {
    for (int i = 0; i < size * size; i++) {
        matrix[i] = rand() % 100;  // Random values between 0 and 99
    }
}

// CPU-bound task: Matrix multiplication
void *cpu_benchmark(void *arg) {
    int *A = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    int *B = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    int *C = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));

    // Generate random matrices
    generate_matrix(A, MATRIX_SIZE);
    generate_matrix(B, MATRIX_SIZE);

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        matrix_multiply(A, B, C, MATRIX_SIZE);  // Perform matrix multiplication
    }

    free(A);
    free(B);
    free(C);
    return NULL;
}

// Disk I/O benchmark function
void disk_benchmark() {
    printf("\n- Running disk I/O benchmark...\n");
    system("dd if=/dev/zero of=testfile bs=1M count=100 oflag=direct");
    system("rm testfile");  // Clean up the file after testing
}

// Producer-consumer implementation
void *producer(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&lock);
        while ((in + 1) % BUFFER_SIZE == out) {
            pthread_cond_wait(&not_full, &lock);
        }
        buffer[in] = i;
        in = (in + 1) % BUFFER_SIZE;
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *consumer(void *arg) {
    int item;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&lock);
        while (in == out) {
            pthread_cond_wait(&not_empty, &lock);
        }
        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

// Benchmark task runner
void run_benchmarks() {
    // CPU benchmark
    printf("==== Running CPU Benchmark ====\n");
    pthread_t cpu_thread;
    clock_t start_time = clock();
    pthread_create(&cpu_thread, NULL, cpu_benchmark, NULL);
    pthread_join(cpu_thread, NULL);
    clock_t end_time = clock();
    double cpu_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("CPU Benchmark completed in %.2f seconds\n", cpu_time);

    // Disk benchmark
    disk_benchmark();

    // Multithreaded Producer-Consumer Benchmark
    printf("==== Running Multithreaded Producer-Consumer Benchmark ====\n");
    pthread_t prod_thread, cons_thread;
    start_time = clock();
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);
    end_time = clock();
    double thread_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Producer-consumer simulation completed in %.2f seconds\n", thread_time);
}

int main() {
    // Run benchmarks
    run_benchmarks();
    return 0;
}
