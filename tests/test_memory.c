#include "test_cpu.h"

void test_opcode_ANNN_set_index(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.memory[0x200] = 0xA4;
    cpu.memory[0x201] = 0x56;
    cpu_cycle(&cpu);
    assert(cpu.I == 0x456);
}

void test_opcode_FX1E_add_to_index(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.I = 0x100;
    cpu.V[0x5] = 0x50;
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x1E;
    cpu_cycle(&cpu);
    assert(cpu.I == 0x150);
    assert(cpu.V[0xF] == 0); /* No overflow */
}

void test_opcode_FX1E_overflow(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.I = 0x0FFF;
    cpu.V[0x5] = 0x02;
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x1E;
    cpu_cycle(&cpu);
    assert(cpu.I == 0x001);
    assert(cpu.V[0xF] == 1); /* Overflow */
}

void test_opcode_FX29_font_character(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x3] = 0xA;
    cpu.memory[0x200] = 0xF3;
    cpu.memory[0x201] = 0x29;
    cpu_cycle(&cpu);
    assert(cpu.I == FONT_LOCATION + 0xA * 5);
}

void test_opcode_FX33_bcd(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 123;
    cpu.I = 0x300;
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x33;
    cpu_cycle(&cpu);
    assert(cpu.memory[0x300] == 1);
    assert(cpu.memory[0x301] == 2);
    assert(cpu.memory[0x302] == 3);
}

void test_opcode_FX33_bcd_edge_cases(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 255;
    cpu.I = 0x300;
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x33;
    cpu_cycle(&cpu);
    assert(cpu.memory[0x300] == 2);
    assert(cpu.memory[0x301] == 5);
    assert(cpu.memory[0x302] == 5);
}

void test_opcode_FX55_reg_dump(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    for (int i = 0; i <= 0x5; i++) {
        cpu.V[i] = i * 10;
    }
    cpu.I = 0x300;
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x55;
    cpu_cycle(&cpu);
    for (int i = 0; i <= 0x5; i++) {
        assert(cpu.memory[0x300 + i] == i * 10);
    }
}

void test_opcode_FX65_reg_load(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.I = 0x300;
    for (int i = 0; i <= 0x5; i++) {
        cpu.memory[0x300 + i] = i * 10;
    }
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x65;
    cpu_cycle(&cpu);
    for (int i = 0; i <= 0x5; i++) {
        assert(cpu.V[i] == i * 10);
    }
}