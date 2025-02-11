#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to search for a specific keyword in /proc/cpuinfo and print matching lines.
void print_cpuinfo_keyword(const char *keyword) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        perror("Error opening /proc/cpuinfo");
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, keyword) != NULL) {
            printf("%s", line);
        }
    }
    fclose(fp);
}

// Function to read and display cache size information from sysfs.
void print_cache_info() {
    char path[128];
    char buffer[64];
    int index = 0;
    
    printf("\nCache Information:\n");
    while (1) {
        snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu0/cache/index%d/size", index);
        FILE *fp = fopen(path, "r");
        if (fp == NULL) {
            // If the file does not exist, assume no more cache info is available.
            break;
        }
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            printf("Cache index %d: %s", index, buffer);
        }
        fclose(fp);
        index++;
    }
}

int main() {
    printf("=== Processor and SoC Information ===\n");

    printf("\n[SoC Details from /proc/cpuinfo]:\n");
    // Print the 'Hardware' field to identify the SoC model.
    print_cpuinfo_keyword("Hardware");
    
    // Print the 'Revision' field to show SoC revision/version information.
    print_cpuinfo_keyword("Revision");
    
    printf("\n[Features from /proc/cpuinfo]:\n");
    // Print the 'Features' field to list the available SoC/board features.
    print_cpuinfo_keyword("Features");
    
    // Display cache size information.
    print_cache_info();
    
    return 0;
}
