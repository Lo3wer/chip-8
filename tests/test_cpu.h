#ifndef TEST_CPU_H
#define TEST_CPU_H

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "cpu.h"

/* Helper to reset CPU to initial state */
static inline void reset_cpu(Cpu *cpu)
{
    cpu_init(cpu);
}

/* Test result tracking */
typedef struct {
    int passed;
    int failed;
    int total;
} TestResults;

static TestResults test_results = {0, 0, 0};

#define RUN_TEST(test_func) \
    do { \
        printf("Running: %s...", #test_func); \
        test_func(); \
        test_results.passed++; \
        test_results.total++; \
        printf(" PASSED\n"); \
    } while(0)

#define PRINT_RESULTS() \
    printf("\n=== Test Results ===\n"); \
    printf("Total: %d\n", test_results.total); \
    printf("Passed: %d\n", test_results.passed); \
    printf("Failed: %d\n", test_results.failed)

#endif