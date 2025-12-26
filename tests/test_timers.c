#include "test_cpu.h"

void test_delay_timer_decrements(void)
{
    Cpu cpu;
    cpu_init(&cpu);
    cpu.delay_timer = 10;
    
    cpu_decrement_timers(&cpu);
    assert(cpu.delay_timer == 9);
    
    cpu_decrement_timers(&cpu);
    assert(cpu.delay_timer == 8);
}

void test_sound_timer_decrements(void)
{
    Cpu cpu;
    cpu_init(&cpu);
    cpu.sound_timer = 5;
    
    cpu_decrement_timers(&cpu);
    assert(cpu.sound_timer == 4);
}

void test_timer_doesnt_go_below_zero(void)
{
    Cpu cpu;
    cpu_init(&cpu);
    cpu.delay_timer = 1;
    
    cpu_decrement_timers(&cpu);
    assert(cpu.delay_timer == 0);
    
    cpu_decrement_timers(&cpu);
    assert(cpu.delay_timer == 0);
}

void test_opcode_FX07_get_delay_timer(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.delay_timer = 42;
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x07;
    cpu_cycle(&cpu);
    assert(cpu.V[0x5] == 42);
}

void test_opcode_FX15_set_delay_timer(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 30;
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x15;
    cpu_cycle(&cpu);
    assert(cpu.delay_timer == 30);
}

void test_opcode_FX18_set_sound_timer(void)
{
    Cpu cpu;
    reset_cpu(&cpu);
    cpu.V[0x5] = 15;
    cpu.memory[0x200] = 0xF5;
    cpu.memory[0x201] = 0x18;
    cpu_cycle(&cpu);
    assert(cpu.sound_timer == 15);
}