#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KB (1024UL)
#define MB (1024UL * KB)
#define GB (1024UL * MB)

/* Function to calculate elapsed time in seconds */
double get_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

/* Q7: Get total available memory */
void memory_info() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        perror("Failed to open /proc/meminfo");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            printf("Total Memory: %s", line + 10);
            break;
        }
    }
    fclose(fp);
}

/* Measure memcpy performance for byte-level (char) copy */
double test_memcpy_bytes(size_t size) {
    char *src = malloc(size);
    char *dest = malloc(size);
    if (!src || !dest) {
        fprintf(stderr, "Memory allocation failed for %zu bytes\n", size);
        exit(EXIT_FAILURE);
    }
    memset(src, 'A', size);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    memcpy(dest, src, size);
    clock_gettime(CLOCK_MONOTONIC, &end);

    free(src);
    free(dest);
    return get_elapsed_time(start, end);
}

/* Measure memcpy performance for half-word (short, 16-bit) copy */
double test_memcpy_halfwords(size_t size) {
    size_t num_elements = size / sizeof(short);
    short *src = malloc(num_elements * sizeof(short));
    short *dest = malloc(num_elements * sizeof(short));
    if (!src || !dest) {
        fprintf(stderr, "Memory allocation failed for %zu bytes\n", size);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < num_elements; i++) src[i] = (short)i;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    memcpy(dest, src, num_elements * sizeof(short));
    clock_gettime(CLOCK_MONOTONIC, &end);

    free(src);
    free(dest);
    return get_elapsed_time(start, end);
}

/* Measure memcpy performance for word (int, 32-bit) copy */
double test_memcpy_words(size_t size) {
    size_t num_elements = size / sizeof(int);
    int *src = malloc(num_elements * sizeof(int));
    int *dest = malloc(num_elements * sizeof(int));
    if (!src || !dest) {
        fprintf(stderr, "Memory allocation failed for %zu bytes\n", size);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < num_elements; i++) src[i] = i;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    memcpy(dest, src, num_elements * sizeof(int));
    clock_gettime(CLOCK_MONOTONIC, &end);

    free(src);
    free(dest);
    return get_elapsed_time(start, end);
}

int main() {
    printf("==== Memory Information (Q7, Q8) ====\n");
    memory_info();

    size_t sizes[] = {KB, MB, GB};
    const char *size_names[] = {"1 KB", "1 MB", "1 GB"};

    printf("\n==== Memory Copy Performance (Q10, Q16) ====\n");
    for (int i = 0; i < 3; i++) {
        size_t current_size = sizes[i];
        printf("\n--- Copying %s ---\n", size_names[i]);

        double time_bytes = test_memcpy_bytes(current_size);
        printf("Bytes (char): %.9f seconds\n", time_bytes);

        double time_halfwords = test_memcpy_halfwords(current_size);
        printf("Half words (short): %.9f seconds\n", time_halfwords);

        double time_words = test_memcpy_words(current_size);
        printf("Words (int): %.9f seconds\n", time_words);
    }

    return 0;
}
