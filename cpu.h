#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 4096
#define PC_START 0x200
#define PC_INCREMENT 2
#define STACK_SIZE 16
#define NUM_REGISTERS 16
#define KEYPAD_SIZE 16
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define SHIFT_SWAP 0 // For 0x8XY6 and 0x8XYE opcodes, 1 = use VY, 0 = use VX
#define DECREMENT_TIMER_MS 1000/60

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    uint8_t V[NUM_REGISTERS];              // 16 general purpose registers
    uint16_t I;                            // Index register
    uint16_t pc;                           // Program counter
    uint16_t stack[STACK_SIZE];            // Stack
    uint8_t sp;                            // Stack pointer
    uint8_t delay_timer;                   // Delay timer
    uint8_t sound_timer;                   // Sound timer
    uint8_t keypad[KEYPAD_SIZE];           // Keypad state
    bool draw_flag;                        // Flag to indicate screen update needed
} Cpu;

bool cpu_init(Cpu *cpu);
void cpu_cycle(Cpu *cpu);
void cpu_decrement_timers(Cpu *cpu);

#endif // CPU_H
