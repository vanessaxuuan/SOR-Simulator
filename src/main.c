#include "../include/sor.h"
#include "../include/config.h"
#include <time.h>
#include <stdio.h>
#include <sys/resource.h>

int main() {
    Exchange exchanges[NUM_EXCHANGES] = {
        {"SGX", 101.5, 5.2, 700, 0.0},
        {"HKEX", 100.2, 6.5, 800, 0.0},
        {"TSE", 99.9, 8.0, 400, 0.0}
    };

    int order_qty = 1500;

    // Initial measurements
    struct timespec start, end; 
    struct rusage usage_start, usage_end;
    
    clock_gettime(CLOCK_MONOTONIC, &start); // Initial time
    getrusage(RUSAGE_SELF, &usage_start); // Initial resource usage

    // Main logic
    // quickSort_calculateScores(exchanges);
    maxHeap_calculateScores(exchanges);
    allocateOrder(exchanges, order_qty);

    // Get Elapsed Time
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsed_ms += (end.tv_nsec - start.tv_nsec) / 1000000.0;
    
    printf("Execution time: %.3f ms\n", elapsed_ms);

    // Get Resource Usage
    getrusage(RUSAGE_SELF, &usage_end);
    long user_sec = usage_end.ru_utime.tv_sec - usage_start.ru_utime.tv_sec;
    long user_usec = usage_end.ru_utime.tv_usec - usage_start.ru_utime.tv_usec;
    double user_ms = user_sec * 1000.0 + user_usec / 1000.0;

    // Calculate system CPU time (in ms)
    long sys_sec = usage_end.ru_stime.tv_sec - usage_start.ru_stime.tv_sec;
    long sys_usec = usage_end.ru_stime.tv_usec - usage_start.ru_stime.tv_usec;
    double sys_ms = sys_sec * 1000.0 + sys_usec / 1000.0;

    // Note: Sum of CPU time should be close to execution time
    printf("User CPU Time: %.3f ms\n", user_ms); // time spent on logic, calculations etc.
    printf("System CPU Time: %.3f ms\n", sys_ms); // time spent on system calls (memory access, I/O e.t.c)

    return 0;
}
