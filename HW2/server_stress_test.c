#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Function to check system uptime, indicating reliability
void check_reliability() {
    printf("\n==== Reliability (Q20) ====\n");

    // Read uptime from /proc/uptime (in seconds)
    FILE *uptime_file = fopen("/proc/uptime", "r");
    if (uptime_file) {
        float uptime_seconds;
        fscanf(uptime_file, "%f", &uptime_seconds);
        fclose(uptime_file);

        float uptime_hours = uptime_seconds / 3600;  // Convert to hours
        printf("System uptime: %.2f hours\n", uptime_hours);

        // Check if uptime is reasonable (example: assume 24+ hours without a reboot means stability)
        if (uptime_hours > 24) {
            printf("Reliability status: Stable (System has been running for %.2f hours)\n", uptime_hours);
        } else {
            printf("Reliability status: Unstable (System has been running for less than 24 hours)\n");
        }
    } else {
        printf("Error: Unable to read uptime.\n");
    }
}

// Run stress test using the `stress` tool for CPU, memory, and IO
void run_stress_test() {
    printf("\n==== Stress Test (Q27) ====\n");

    // CPU Stress Test: Run 4 workers stressing the CPU for 60 seconds
    printf("Starting CPU stress test...\n");
    system("stress --cpu 4 --timeout 60s");

    // Memory Stress Test: Run 1 worker stressing memory with 1GB of memory for 60 seconds
    printf("Starting memory stress test...\n");
    system("stress --vm 1 --vm-bytes 1G --timeout 60s");

    // Disk Stress Test: Simulate disk I/O by writing a large file
    printf("Starting disk stress test...\n");
    system("stress --io 4 --timeout 60s");

    printf("Stress test completed.\n");
}

// Get the power consumption (using vcgencmd for voltage and temperature)
void get_power_consumption() {
    printf("\n==== Power Consumption (Q26) ====\n");

    // Placeholder for actual power consumption tools (using vcgencmd for voltage)
    printf("Voltage (a rough estimate of power consumption):\n");
    system("vcgencmd measure_volts");  // Output voltage info

    // Getting the CPU temperature as a rough indicator of load (indirect link to power)
    printf("Current CPU temperature:\n");
    system("vcgencmd measure_temp");   // Get the current temperature of the CPU as a rough indicator
}

// Monitor temperature and check if it affects performance
void get_temperature_effect() {
    printf("==== Temperature Effect on Performance (Q28) ====\n");

    // Read the temperature file from Raspberry Pi
    FILE *temp_file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (temp_file) {
        int temperature;
        fscanf(temp_file, "%d", &temperature);
        fclose(temp_file);

        // Convert temperature from millidegrees Celsius to degrees Celsius
        temperature = temperature / 1000;

        printf("Current CPU temperature: %d°C\n", temperature);

        // Check if the temperature exceeds safe thresholds
        if (temperature > 80) {  // If temperature exceeds 80°C, the system might throttle
            printf("Warning: High temperature (%d°C) detected. Performance may degrade due to throttling.\n", temperature);
        } else {
            printf("Temperature is within acceptable limits: %d°C.\n", temperature);
        }
    } else {
        printf("Error: Unable to read temperature file.\n");
    }
}

// Print basic system information
void get_system_info() {
    printf("\n==== System Information ====\n");

    // Use uname command to fetch OS information
    system("uname -srm");  // Prints OS name, kernel version, and architecture

    // Output basic hardware info (e.g., CPU model)
    system("cat /proc/cpuinfo | grep 'model name'");  // Prints CPU model
}

int main() {
    get_system_info();        // Get basic system info
    check_reliability();      // Check system reliability and uptime
    get_power_consumption();  // Check power consumption
    run_stress_test();        // Run stress test using the `stress` tool
    get_temperature_effect(); // Check the effect of temperature on performance

    return 0;
}
