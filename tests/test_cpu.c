#include <assert.h>
#include <string.h>
#include "cpu.h"

/* Helpers */
static void reset_cpu(Cpu *cpu)
{
    cpu_init(cpu);
}

static void test_opcode_6XNN_sets_register()
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.memory[0x200] = 0x6A; /* 6XNN: X=A */
    cpu.memory[0x201] = 0x42; /* NN=0x42 */
    cpu_cycle(&cpu);
    assert(cpu.V[0xA] == 0x42);
    assert(cpu.pc == 0x202);
}

static void test_opcode_7XNN_adds_to_register()
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 0x10;
    cpu.memory[0x200] = 0x75; /* 7XNN: X=5 */
    cpu.memory[0x201] = 0x20; /* NN=0x20 */
    cpu_cycle(&cpu);
    assert(cpu.V[0x5] == 0x30);
    assert(cpu.pc == 0x202);
}

static void test_opcode_00E0_clears_screen()
{
    Cpu cpu;
    reset_cpu(&cpu);
    memset(cpu.display, 1, sizeof(cpu.display));
    cpu.memory[0x200] = 0x00; /* 00E0 */
    cpu.memory[0x201] = 0xE0;
    cpu_cycle(&cpu);
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
    {
        assert(cpu.display[i] == 0);
    }
    assert(cpu.draw_flag == true);
    assert(cpu.pc == 0x202);
}

static void test_opcode_1NNN_jumps()
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.memory[0x200] = 0x12; /* 1NNN */
    cpu.memory[0x201] = 0x34; /* NNN=0x234 */
    cpu_cycle(&cpu);
    assert(cpu.pc == 0x234);
}

static void test_opcode_2NNN_call_and_00EE_return()
{
    Cpu cpu;
    reset_cpu(&cpu);
    /* Call 0x300 */
    cpu.memory[0x200] = 0x23; /* 2NNN */
    cpu.memory[0x201] = 0x00; /* NNN=0x300 */
    /* At 0x300: Return */
    cpu.memory[0x300] = 0x00; /* 00EE */
    cpu.memory[0x301] = 0xEE;

    cpu_cycle(&cpu);
    assert(cpu.pc == 0x300);
    assert(cpu.sp == 1);
    assert(cpu.stack[0] == 0x200);

    cpu_cycle(&cpu);
    assert(cpu.sp == 0);
    assert(cpu.pc == 0x202); /* back to caller + 2 */
}

static void test_opcode_DXYN_draw_sprite_no_collision()
{
    Cpu cpu;
    reset_cpu(&cpu);
    /* Prepare a 1-byte sprite: 0xF0 -> 11110000 */
    cpu.I = 0x400; /* arbitrary location */
    cpu.memory[cpu.I] = 0xF0;
    /* Place sprite at (x=1, y=2) using V1 and V2 */
    cpu.V[0x1] = 1;
    cpu.V[0x2] = 2;
    /* DXYN: D 1 2 1 => draw 1 row from I */
    cpu.memory[0x200] = 0xD1; /* high nibble D, X=1 */
    cpu.memory[0x201] = 0x21; /* Y=2, N=1 */

    cpu_cycle(&cpu);

    /* Verify 4 pixels set starting at (1,2): bits 1111 0000 */
    int y = 2;
    for (int col = 0; col < 8; col++)
    {
        int x = 1 + col;
        int idx = y * DISPLAY_WIDTH + (x % DISPLAY_WIDTH);
        int expected = (col < 4) ? 1 : 0;
        assert(cpu.display[idx] == expected);
    }
    assert(cpu.V[0xF] == 0);
    assert(cpu.draw_flag == true);
}

static void test_opcode_DXYN_draw_sprite_with_collision()
{
    Cpu cpu;
    reset_cpu(&cpu);
    /* Prepare sprite */
    cpu.I = 0x450;
    cpu.memory[cpu.I] = 0x80; /* 1000 0000 */
    cpu.V[0x3] = 0;           /* x */
    cpu.V[0x4] = 0;           /* y */

    /* Pre-set pixel at (0,0) to 1 to trigger collision */
    cpu.display[0] = 1;

    /* D 3 4 1 */
    cpu.memory[0x200] = 0xD3;
    cpu.memory[0x201] = 0x41;

    cpu_cycle(&cpu);

    /* Pixel (0,0) should XOR to 0, collision flag set */
    assert(cpu.display[0] == 0);
    assert(cpu.V[0xF] == 1);
}

int main(void)
{
    test_opcode_6XNN_sets_register();
    test_opcode_7XNN_adds_to_register();
    test_opcode_00E0_clears_screen();
    test_opcode_1NNN_jumps();
    test_opcode_2NNN_call_and_00EE_return();
    test_opcode_DXYN_draw_sprite_no_collision();
    test_opcode_DXYN_draw_sprite_with_collision();
    return 0;
}
