#include <string.h>
#include <SDL3/SDL.h>
#include "cpu.h"

/* CHIP-8 fontset (0-F) - stored in memory at 0x50-0x9F */
static const uint8_t fontset[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

bool cpu_init(Cpu *cpu)
{
    memset(cpu, 0, sizeof(Cpu));
    cpu->pc = PC_START;
    cpu->sp = 0;
    cpu->I = 0;
    cpu->draw_flag = false;

    /* Load fontset into memory at 0x50 */
    memcpy(&cpu->memory[0x50], fontset, sizeof(fontset));

    return true;
}

void cpu_cycle(Cpu *cpu)
{
    /* Fetch opcode (2 bytes, big-endian) */
    uint16_t opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];

    /* Extract common opcode parts */
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t N = opcode & 0x000F;
    uint8_t NN = opcode & 0x00FF;
    uint16_t NNN = opcode & 0x0FFF;

    /* Decode and execute */
    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) {
                /* 00E0 - Clear screen */
                memset(cpu->display, 0, sizeof(cpu->display));
                cpu->draw_flag = true;
                cpu->pc += PC_INCREMENT;
            }
            else if (opcode == 0x00EE) {
                /* 00EE - Return from subroutine */
                cpu->sp--;
                cpu->pc = cpu->stack[cpu->sp];
                cpu->pc += PC_INCREMENT;
            }
            break;

        case 0x1000:
            /* 1NNN - Jump to address NNN */
            cpu->pc = NNN;
            break;

        case 0x6000:
            /* 6XNN - Set register VX to NN */
            cpu->V[X] = NN;
            cpu->pc += PC_INCREMENT;
            break;

        case 0x7000:
            /* 7XNN - Add NN to register VX (no carry flag) */
            cpu->V[X] += NN;
            cpu->pc += PC_INCREMENT;
            break;

        case 0xA000:
            /* ANNN - Set index register I to NNN */
            cpu->I = NNN;
            cpu->pc += PC_INCREMENT;
            break;

        case 0xD000:
            /* DXYN - Draw sprite at (VX, VY) with height N */
            {
                uint8_t x = cpu->V[X] % DISPLAY_WIDTH;
                uint8_t y = cpu->V[Y] % DISPLAY_HEIGHT;
                cpu->V[0xF] = 0; // Reset collision flag

                for (uint8_t row = 0; row < N; row++) {
                    uint8_t sprite_byte = cpu->memory[cpu->I + row];
                    
                    for (uint8_t col = 0; col < 8; col++) {
                        if ((sprite_byte & (0x80 >> col)) != 0) {
                            uint16_t pixel_x = (x + col) % DISPLAY_WIDTH;
                            uint16_t pixel_y = (y + row) % DISPLAY_HEIGHT;
                            uint16_t pixel_index = pixel_y * DISPLAY_WIDTH + pixel_x;

                            /* Check for collision */
                            if (cpu->display[pixel_index] == 1) {
                                cpu->V[0xF] = 1;
                            }

                            /* XOR the pixel */
                            cpu->display[pixel_index] ^= 1;
                        }
                    }
                }

                cpu->draw_flag = true;
                cpu->pc += PC_INCREMENT;
            }
            break;

        default:
            SDL_Log("Unknown opcode: 0x%X at PC: 0x%X", opcode, cpu->pc);
            cpu->pc += PC_INCREMENT;
            break;
    }
}