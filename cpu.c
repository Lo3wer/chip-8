#include <string.h>
#include <stdio.h>
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
    memcpy(&cpu->memory[FONT_LOCATION], fontset, sizeof(fontset));

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

    /* Increment program counter*/
    cpu->pc += PC_INCREMENT;

    /* Decode and execute */
    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) {
                /* 00E0 - Clear screen */
                memset(cpu->display, 0, sizeof(cpu->display));
                cpu->draw_flag = true;
            }
            else if (opcode == 0x00EE) {
                /* 00EE - Return from subroutine */
                cpu->sp--;
                cpu->pc = cpu->stack[cpu->sp];
            }
            else {
                fprintf(stderr, "Unknown opcode: 0x%X at PC: 0x%X\n", opcode, cpu->pc);
            }
            break;

        case 0x1000:
            /* 1NNN - Jump to address NNN */
            cpu->pc = NNN;
            break;
        
        case 0x2000:
            /* 2NNN - Call subroutine at NNN */
            cpu->stack[cpu->sp] = cpu->pc;
            cpu->sp++;
            cpu->pc = NNN;
            break;

        case 0x3000:
            /* 3XNN - Skip next instruction if VX == NN */
            cpu->pc += (cpu->V[X] == NN) ? PC_INCREMENT : 0;
            break;

        case 0x4000:
            /* 4XNN - Skip next instruction if VX != NN */
            cpu->pc += (cpu->V[X] != NN) ? PC_INCREMENT : 0;
            break;

        case 0x5000:
            /* 5XY0 - Skip next instruction if VX == VY */
            cpu->pc += (cpu->V[X] == cpu->V[Y]) ? PC_INCREMENT : 0;
            break;

        case 0x6000:
            /* 6XNN - Set register VX to NN */
            cpu->V[X] = NN;
            break;

        case 0x7000:
            /* 7XNN - Add NN to register VX (no carry flag) */
            cpu->V[X] += NN;
            break;

        case 0x8000:
            switch (N) {
                case 0x0000:
                    /* 8XY0 - Set VX = VY */
                    cpu->V[X] = cpu->V[Y];
                    break;
                case 0x0001:
                    /* 8XY1 - Set VX = VX OR VY */
                    cpu->V[X] |= cpu->V[Y];
                    break;
                case 0x0002:
                    /* 8XY2 - Set VX = VX AND VY */
                    cpu->V[X] &= cpu->V[Y];
                    break;
                case 0x0003:
                    /* 8XY3 - Set VX = VX XOR VY */
                    cpu->V[X] ^= cpu->V[Y];
                    break;
                case 0x0004:
                    /* 8XY4 - Add VY to VX, set VF = carry */
                    {
                        uint16_t sum = cpu->V[X] + cpu->V[Y];
                        cpu->V[0xF] = (sum > 0xFF) ? 1 : 0;
                        cpu->V[X] = sum & 0xFF;
                    }
                    break;
                case 0x0005:
                    /* 8XY5 - Subtract VY from VX, set VF = NOT borrow */
                    cpu->V[0xF] = (cpu->V[X] >= cpu->V[Y]) ? 1 : 0;
                    cpu->V[X] -= cpu->V[Y];
                    break;
                case 0x0007:
                    /* 8XY7 - Set VX = VY - VX, set VF = NOT borrow */
                    cpu->V[0xF] = (cpu->V[Y] >= cpu->V[X]) ? 1 : 0;
                    cpu->V[X] = cpu->V[Y] - cpu->V[X];
                    break;
                case 0x0006:
                    /* 8XY6 - Shift VX right by 1, set VF = least significant bit before shift */
                    if(SHIFT_SWAP){ //Will set VX to VY before shifting if SHIFT_SWAP is set to 1
                        cpu->V[X] = cpu->V[Y];
                    }
                    cpu->V[0xF] = cpu->V[X] & 0x1;
                    cpu->V[X] >>= 1;
                    break;
                case 0x000E:
                    /* 8XYE - Shift VX left by 1, set VF = most significant bit before shift */
                    if(SHIFT_SWAP){ //Will set VX to VY before shifting if SHIFT_SWAP is set to 1
                        cpu->V[X] = cpu->V[Y];
                    }
                    cpu->V[0xF] = (cpu->V[X] & 0x80) >> 7;
                    cpu->V[X] <<= 1;
                    break;
                default:
                    fprintf(stderr, "Unknown opcode: 0x%X at PC: 0x%X\n", opcode, cpu->pc);
                    break;
            }
            break;

        case 0x9000:
            /* 9XY0 - Skip next instruction if VX != VY */
            cpu->pc += (cpu->V[X] != cpu->V[Y]) ? PC_INCREMENT : 0;
            break;

        case 0xA000:
            /* ANNN - Set index register I to NNN */
            cpu->I = NNN;
            break;

        case 0xB000:
            /* BNNN - Jump to address NNN + V0 */
            cpu->pc = NNN + cpu->V[0];
            break;
        
        case 0xC000:
            /* CXNN - Set VX = random byte AND NN */
            uint8_t rand_byte = (uint8_t)(rand() % 256);
            cpu->V[X] = rand_byte & NN;
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
            }
            break;

        case 0xE000:
            if (NN == 0x9E) {
                /* EX9E - Skip next instruction if key with the value of VX is pressed */
                cpu->pc += (cpu->keypad[cpu->V[X]] != 0) ? PC_INCREMENT : 0;
            } else if (NN == 0xA1) {
                /* EXA1 - Skip next instruction if key with the value of VX is not pressed */
                cpu->pc += (cpu->keypad[cpu->V[X]] == 0) ? PC_INCREMENT : 0;
            } else {
                fprintf(stderr, "Unknown opcode: 0x%X at PC: 0x%X\n", opcode, cpu->pc);
                cpu->pc += PC_INCREMENT;
            }
            break;

        case 0xF000:
            switch (NN) {
                case 0x0007:
                    /* FX07 - Set VX to delay_timer */
                    cpu->V[X] = cpu->delay_timer;
                    break;
                case 0x0015:
                    /* FX15 - Set delay_timer to VX*/
                    cpu->delay_timer = cpu->V[X];
                    break;
                case 0x0018:
                    /* FX18 - Set sound_timer to VX*/
                    cpu->sound_timer = cpu->V[X];
                    break;
                case 0x001E:
                    /* FX1E - Set index register to VX and track overflow with VF*/
                    cpu->I += cpu->V[X];
                    cpu->V[0xF] = (cpu->I > 0x0FFF) ? 1 : 0;
                    cpu->I &= 0x0FFF;
                    break;
                case 0x000A:
                    /* FX0A - Blocks until certain key is pressed*/
                    bool key_found = false;
                    for(int i = 0; i < KEYPAD_SIZE; i++){
                        if(cpu->keypad[i] != 0){
                            cpu->V[X] = i;
                            key_found = true;
                            break;
                        }
                    }
                    cpu->pc -= (!key_found) ? PC_INCREMENT : 0;
                    break;
                case 0x0029:
                    /* FX29 - Sets I to the mem address of the hex character in VX*/
                    cpu->I = FONT_LOCATION + (cpu->V[X] & 0xF)*5;
                    break;
                case 0x0033:
                    /* FX33 - Sets three decimals at mem address I with VX*/
                    uint8_t value = cpu->V[X];
                    if(cpu->I<MEMORY_SIZE-2){
                        cpu->memory[cpu->I + 2] = value%10;
                        cpu->memory[cpu->I + 1] = (value/10)%10;
                        cpu->memory[cpu->I] = value/100;
                    }
                    break;
                case 0x0055:
                    /* FX55 - Sets the memory I to I + X to registers V0-VX respectively*/
                    if(cpu->I + X < MEMORY_SIZE){
                        for(int i = 0; i <= X; i++){
                            cpu->memory[cpu->I + i] = cpu->V[i];
                        }
                        cpu->I += (INDEX_UPDATE) ? X + 1 : 0;
                    }
                    break;
                case 0x0065:
                    /* FX65 - Reads the memory I to I + X and sets registers V0-VX respectively*/
                    if(cpu->I + X < MEMORY_SIZE){
                        for(int i = 0; i <= X; i++){
                            cpu->V[i] = cpu->memory[cpu->I + i];
                        }
                        cpu->I += (INDEX_UPDATE) ? X + 1 : 0;
                    }
                    break;
                default:
                    fprintf(stderr, "Unknown opcode: 0x%X at PC: 0x%X\n", opcode, cpu->pc);
                    break;
            }
            break;
            
        default:
            fprintf(stderr, "Unknown opcode: 0x%X at PC: 0x%X\n", opcode, cpu->pc);
            break;
    }
}

void cpu_decrement_timers(Cpu *cpu)
{
    /* Decrement timers at 60Hz */
    if (cpu->delay_timer > 0) {
        cpu->delay_timer--;
    }

    if (cpu->sound_timer > 0) {
        cpu->sound_timer--;
    }
}