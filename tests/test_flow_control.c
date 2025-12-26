#include "test_cpu.h"

void test_opcode_1NNN_jumps(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.memory[0x200] = 0x12;
    cpu.memory[0x201] = 0x34;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x234);
}

void test_opcode_2NNN_call_and_00EE_return(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.memory[0x200] = 0x23;
    cpu.memory[0x201] = 0x00;
    cpu.memory[0x300] = 0x00;
    cpu.memory[0x301] = 0xEE;

    cpu_cycle(&cpu);
    assert(cpu.pc == 0x300);
    assert(cpu.sp == 1);
    assert(cpu.stack[0] == 0x202);

    cpu_cycle(&cpu);
    assert(cpu.sp == 0);
    assert(cpu.pc == 0x202);
}

void test_opcode_BNNN_jump_with_offset(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0] = 0x10;
    cpu.memory[0x200] = 0xB2;
    cpu.memory[0x201] = 0x00;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x210); /* 0x200 + 0x10 */
}

void test_opcode_3XNN_skip_if_equal(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0x42;
    cpu.memory[0x200] = 0x35;
    cpu.memory[0x201] = 0x42;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x204); /* Skipped */
}

void test_opcode_3XNN_no_skip_if_not_equal(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0x42;
    cpu.memory[0x200] = 0x35;
    cpu.memory[0x201] = 0x43;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x202); /* Not skipped */
}

void test_opcode_4XNN_skip_if_not_equal(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0x42;
    cpu.memory[0x200] = 0x45;
    cpu.memory[0x201] = 0x43;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x204); /* Skipped */
}

void test_opcode_4XNN_no_skip_if_equal(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0x42;
    cpu.memory[0x200] = 0x45;
    cpu.memory[0x201] = 0x42;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x202); /* Not skipped */
}

void test_opcode_5XY0_skip_if_registers_equal(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x42;
    cpu.V[0x7] = 0x42;
    cpu.memory[0x200] = 0x53;
    cpu.memory[0x201] = 0x70;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x204); /* Skipped */
}

void test_opcode_9XY0_skip_if_registers_not_equal(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x42;
    cpu.V[0x7] = 0x43;
    cpu.memory[0x200] = 0x93;
    cpu.memory[0x201] = 0x70;
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x204); /* Skipped */
}