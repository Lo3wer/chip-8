#include "test_cpu.h"

void test_opcode_EX9E_skip_if_key_pressed(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0x3;
    cpu.keypad[0x3] = 1;
    cpu.memory[0x200] = 0xE5;
    cpu.memory[0x201] = 0x9E;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x204); /* Skipped */
}

void test_opcode_EX9E_no_skip_if_key_not_pressed(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0x3;
    cpu.keypad[0x3] = 0;
    cpu.memory[0x200] = 0xE5;
    cpu.memory[0x201] = 0x9E;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x202); /* Not skipped */
}

void test_opcode_EXA1_skip_if_key_not_pressed(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0x3;
    cpu.keypad[0x3] = 0;
    cpu.memory[0x200] = 0xE5;
    cpu.memory[0x201] = 0xA1;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x204); /* Skipped */
}

void test_opcode_FX0A_wait_for_key(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x0A;
    
    /* No key pressed - should stay at same PC */
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x200);
    
    /* Press key 7 */
    cpu.keypad[0x7] = 1;
    cpu_cycle(&cpu);
    assert(cpu.V[0x5] == 0x7);
    assert(cpu.pc == 0x202);
}