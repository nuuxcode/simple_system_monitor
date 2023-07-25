#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROCESSES 100
#define MAX_PROC_NAME 256

typedef struct {
    int pid;
    char name[MAX_PROC_NAME];
    unsigned long cpu_usage;
    unsigned long memory_usage;
} ProcessInfo;

void get_cpu_info(unsigned long* total, unsigned long* free) {
    FILE* fp = fopen("/proc/stat", "r");
    fscanf(fp, "cpu %lu %*lu %lu", total, free);
    fclose(fp);
}

void get_memory_info(unsigned long* total, unsigned long* free) {
    FILE* fp = fopen("/proc/meminfo", "r");
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strncmp(buffer, "MemTotal:", 9) == 0) {
            sscanf(buffer + 9, "%lu", total);
        } else if (strncmp(buffer, "MemFree:", 8) == 0) {
            sscanf(buffer + 8, "%lu", free);
            break;
        }
    }

    fclose(fp);
}

int get_process_list(ProcessInfo* process_list) {
    int count = 0;
    FILE* fp = popen("ps -e -o pid,comm", "r");

    if (fp == NULL) {
        perror("popen");
        return -1;
    }

    while (count < MAX_PROCESSES && fscanf(fp, "%d %[^\n]", &process_list[count].pid, process_list[count].name) == 2) {
        count++;
    }

    pclose(fp);
    return count;
}

void get_process_usage(ProcessInfo* process_list, int count) {
    for (int i = 0; i < count; i++) {
        char filename[256];
        sprintf(filename, "/proc/%d/stat", process_list[i].pid);

        FILE* fp = fopen(filename, "r");
        if (fp == NULL) {
            perror("fopen");
            continue;
        }

        fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %lu %lu", &process_list[i].cpu_usage, &process_list[i].memory_usage);
        fclose(fp);
    }
}

void display_stats(unsigned long total_cpu, unsigned long free_cpu, unsigned long total_memory, unsigned long free_memory, ProcessInfo* process_list, int process_count) {
    system("clear");

    printf("===== CPU Usage =====\n");
    printf("Overall Usage: %lu%%\n", 100 - (free_cpu * 100 / total_cpu));
    printf("Overall Free: %lu%%\n\n", (free_cpu * 100 / total_cpu));

    printf("===== Memory Usage =====\n");
    printf("Total: %lu KB\n", total_memory);
    printf("Free: %lu KB\n", free_memory);
    printf("Usage: %lu KB\n\n", total_memory - free_memory);

    printf("===== Process List =====\n");
    for (int i = 0; i < process_count; i++) {
        printf("[%d] %s\n", process_list[i].pid, process_list[i].name);
        printf("   CPU Usage: %lu\n", process_list[i].cpu_usage);
        printf("   Memory Usage: %lu\n", process_list[i].memory_usage);
    }

    printf("\nPress Ctrl+C to exit...\n");
}

int main() {
    unsigned long total_cpu, free_cpu;
    unsigned long total_memory, free_memory;
    ProcessInfo process_list[MAX_PROCESSES];
    int process_count;

    while (1) {
        get_cpu_info(&total_cpu, &free_cpu);
        get_memory_info(&total_memory, &free_memory);
        process_count = get_process_list(process_list);
        get_process_usage(process_list, process_count);
        display_stats(total_cpu, free_cpu, total_memory, free_memory, process_list, process_count);
        sleep(5); // Refresh rate of 5 seconds
    }

    return 0;
}
