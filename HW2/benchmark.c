#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Function to run benchmarks
void run_benchmarks() {
    printf("==== Benchmarking System Performance (Q21) ====\n");
    // fflush(stdout);  // Ensure output is flushed immediately

    printf("\n- CPU and memory usage:\n");
    system("top -n 1 | head -10");  // Show CPU and memory usage

    printf("\n- System performance stats (CPU and I/O utilization):\n");
    system("iostat -x 1 5");  // CPU utilization and IO stats

    printf("\n- Disk performance:\n");
    system("hdparm -Tt /dev/mmcblk0");  // Benchmark disk performance for Raspberry Pi
}

int main() {
    // Running benchmarks
    run_benchmarks();

    return 0;
}
