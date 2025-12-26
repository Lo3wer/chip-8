#include "test_cpu.h"

void test_opcode_6XNN_sets_register(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.memory[0x200] = 0x6A;
    cpu.memory[0x201] = 0x42;
    cpu_cycle(&cpu);
    assert(cpu.V[0xA] == 0x42);
    assert(cpu.pc == 0x202);
}

void test_opcode_7XNN_adds_to_register(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0x10;
    cpu.memory[0x200] = 0x75;
    cpu.memory[0x201] = 0x20;
    cpu_cycle(&cpu);
    assert(cpu.V[0x5] == 0x30);
    assert(cpu.pc == 0x202);
}

void test_opcode_7XNN_wraps_around(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0xFF;
    cpu.memory[0x200] = 0x75;
    cpu.memory[0x201] = 0x02;
    cpu_cycle(&cpu);
    assert(cpu.V[0x5] == 0x01); /* Wraps */
}

void test_opcode_8XY0_assign(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x42;
    cpu.V[0x7] = 0x99;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x70;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0x99);
}

void test_opcode_8XY1_or(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x0F;
    cpu.V[0x7] = 0xF0;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x71;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0xFF);
}

void test_opcode_8XY2_and(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x0F;
    cpu.V[0x7] = 0xFF;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x72;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0x0F);
}

void test_opcode_8XY3_xor(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0xFF;
    cpu.V[0x7] = 0xFF;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x73;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0x00);
}

void test_opcode_8XY4_add_with_carry(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0xFF;
    cpu.V[0x7] = 0x02;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x74;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0x01);
    assert(cpu.V[0xF] == 1); /* Carry */
}

void test_opcode_8XY4_add_no_carry(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x10;
    cpu.V[0x7] = 0x20;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x74;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0x30);
    assert(cpu.V[0xF] == 0); /* No carry */
}

void test_opcode_8XY5_sub_no_borrow(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x50;
    cpu.V[0x7] = 0x30;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x75;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0x20);
    assert(cpu.V[0xF] == 1); /* No borrow */
}

void test_opcode_8XY5_sub_with_borrow(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x30;
    cpu.V[0x7] = 0x50;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x75;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0xE0); /* Wraps */
    assert(cpu.V[0xF] == 0); /* Borrow */
}

void test_opcode_8XY7_subn_no_borrow(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x30;
    cpu.V[0x7] = 0x50;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x77;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0x20);
    assert(cpu.V[0xF] == 1); /* No borrow */
}

void test_opcode_8XY6_shift_right(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0x0F;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x76;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0x07);
    assert(cpu.V[0xF] == 1); /* LSB was 1 */
}

void test_opcode_8XYE_shift_left(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0xF0;
    cpu.memory[0x200] = 0x83;
    cpu.memory[0x201] = 0x7E;
    cpu_cycle(&cpu);
    assert(cpu.V[0x3] == 0xE0);
    assert(cpu.V[0xF] == 1); /* MSB was 1 */
}