#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void get_power_consumption() {
    printf("==== Power Consumption (Q26) ====\n");

    // Placeholder for getting power consumption (simulate without load and with load)
    printf("Power consumption without load: 20W\n");  // Example value
    printf("Power consumption with load: 50W\n");    // Example value
}

void get_temperature_effect() {
    printf("\n==== Temperature Effect on Performance (Q28) ====\n");

    // Simulating temperature effect
    int temperature = 40;  // Example temperature value in Celsius
    if (temperature > 70) {
        printf("Warning: High temperature (%d°C) detected. Performance may degrade.\n", temperature);
    } else {
        printf("Temperature is within acceptable limits: %d°C.\n", temperature);
    }
}

void stress_test() {
    printf("\n==== Stress Test (Q27) ====\n");

    // Simulating a stress test by running a heavy loop
    for (long i = 0; i < 1000000000; i++) {
        // Perform some heavy computation to simulate stress
        if (i % 100000000 == 0) {
            printf("Stress test in progress... Iteration: %ld\n", i);
        }
    }
    printf("Stress test completed.\n");
}

void check_reliability() {
    printf("\n==== Reliability (Q20) ====\n");

    // Simulating a reliability check
    // Here we could check for system stability over time (e.g., uptime, error logs)
    printf("System uptime: 1200 hours\n");  // Example uptime value
    printf("Reliability status: Stable\n");
}

int main() {
    check_reliability();
    get_power_consumption();
    stress_test();
    get_temperature_effect();
    
    return 0;
}
