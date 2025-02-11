#include <stdio.h>
#include <stdlib.h>

void get_boot_time() {
    printf("==== Boot Time Analysis (Q18) ====\n");
    system("systemd-analyze");  // Shows total boot time
    system("systemd-analyze blame | head -10");  // Shows top 10 services affecting boot time
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_ITERATIONS 1000000

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int turn = 0;

struct timespec diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

void *thread_func(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&lock);
        while (turn != 1) {
            pthread_cond_wait(&cond, &lock);
        }
        turn = 0;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void measure_context_switch() {
    printf("\n==== Context Switch Time Measurement (Q13) ====\n");

    pthread_t thread;
    struct timespec start, end, result;

    pthread_create(&thread, NULL, thread_func, NULL);

    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&lock);
        turn = 1;
        pthread_cond_signal(&cond);
        while (turn != 0) {
            pthread_cond_wait(&cond, &lock);
        }
        pthread_mutex_unlock(&lock);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    pthread_join(thread, NULL);

    result = diff(start, end);
    double total_time_ns = (result.tv_sec * 1e9) + result.tv_nsec;
    double avg_time_ns = total_time_ns / NUM_ITERATIONS;
    
    printf("Average context switch time: %.2f ns\n", avg_time_ns);
}

int main() {
    get_boot_time();
    measure_context_switch();
    return 0;
}