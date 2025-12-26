#include "test_cpu.h"
#include <stdlib.h>

void test_opcode_CXNN_random(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    srand(12345); /* Fixed seed for reproducibility */
    cpu.memory[0x200] = 0xC5;
    cpu.memory[0x201] = 0x0F;
    cpu_cycle(&cpu);
    /* Value should be random & 0x0F, so <= 0x0F */
    assert(cpu.V[0x5] <= 0x0F);
}