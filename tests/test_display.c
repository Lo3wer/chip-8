#include "test_cpu.h"

void test_opcode_00E0_clears_screen(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    memset(cpu.display, 1, sizeof(cpu.display));
    cpu.memory[0x200] = 0x00;
    cpu.memory[0x201] = 0xE0;
    cpu_cycle(&cpu);
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        assert(cpu.display[i] == 0);
    }
    assert(cpu.draw_flag == true);
    assert(cpu.pc == 0x202);
}

void test_opcode_DXYN_draw_sprite_no_collision(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.I = 0x400;
    cpu.memory[cpu.I] = 0xF0; /* 11110000 */
    cpu.V[0x1] = 1;
    cpu.V[0x2] = 2;
    cpu.memory[0x200] = 0xD1;
    cpu.memory[0x201] = 0x21;

    cpu_cycle(&cpu);

    int y = 2;
    for (int col = 0; col < 8; col++) {
        int x = 1 + col;
        int idx = y * DISPLAY_WIDTH + (x % DISPLAY_WIDTH);
        int expected = (col < 4) ? 1 : 0;
        assert(cpu.display[idx] == expected);
    }
    assert(cpu.V[0xF] == 0);
    assert(cpu.draw_flag == true);
}

void test_opcode_DXYN_draw_sprite_with_collision(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.I = 0x450;
    cpu.memory[cpu.I] = 0x80;
    cpu.V[0x3] = 0;
    cpu.V[0x4] = 0;
    cpu.display[0] = 1;

    cpu.memory[0x200] = 0xD3;
    cpu.memory[0x201] = 0x41;

    cpu_cycle(&cpu);

    assert(cpu.display[0] == 0);
    assert(cpu.V[0xF] == 1);
}

void test_opcode_DXYN_wraps_around_screen(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.I = 0x400;
    cpu.memory[cpu.I] = 0xFF;
    cpu.V[0x1] = DISPLAY_WIDTH - 2;
    cpu.V[0x2] = DISPLAY_HEIGHT - 1;
    cpu.memory[0x200] = 0xD1;
    cpu.memory[0x201] = 0x21;

    cpu_cycle(&cpu);

    /* Should wrap to left side */
    assert(cpu.display[(DISPLAY_HEIGHT - 1) * DISPLAY_WIDTH + (DISPLAY_WIDTH - 2)] == 1);
    assert(cpu.display[(DISPLAY_HEIGHT - 1) * DISPLAY_WIDTH + 0] == 1);
}