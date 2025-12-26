#define SDL_MAIN_USE_CALLBACKS 1
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "cpu.h"
#include "display.h"
#include "audio.h"

typedef struct
{
    Cpu cpu;
    Display display;
    Audio audio;
    bool running;
} Chip8Emulator;

/* Load a CHIP-8 program from file */
bool load_program(Cpu *cpu, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        SDL_Log("Failed to open program file: %s", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size > (MEMORY_SIZE - PC_START)) //0x200 is the start address for programs
    {
        SDL_Log("Program file too large: %ld bytes", file_size);
        fclose(file);
        return false;
    }

    size_t bytes_read = fread(&cpu->memory[PC_START], 1, file_size, file);
    fclose(file);

    if (bytes_read != (size_t)file_size)
    {
        SDL_Log("Failed to read program file");
        return false;
    }

    SDL_Log("Loaded program: %ld bytes", file_size);
    return true;
}

/* Initialize the app */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    Chip8Emulator *state = SDL_malloc(sizeof(Chip8Emulator));
    if (!state)
        return SDL_APP_FAILURE;

    state->running = true;

    /* Initialize CPU */
    cpu_init(&state->cpu);

    /* Initialize display */
    if (!display_init(&state->display))
    {
        SDL_Log("Failed to initialize display");
        SDL_free(state);
        return SDL_APP_FAILURE;
    }

    /* Initialize audio */
    if (!audio_init(&state->audio))
    {
        SDL_Log("Failed to initialize audio");
        display_cleanup(&state->display);
        SDL_free(state);
        return SDL_APP_FAILURE;
    }

    /* Load program from command line argument or default */
    const char *program = (argc > 1) ? argv[1] : "C:/Users/TestUser/chip-8/programs/Ibmlogo.ch8";
    if (!load_program(&state->cpu, program))
    {
        SDL_Log("Failed to load program");
        display_cleanup(&state->display);
        SDL_free(state);
        return SDL_APP_FAILURE;
    }

    *appstate = state;
    return SDL_APP_CONTINUE;
}

/* Handle input events */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    Chip8Emulator *state = (Chip8Emulator *)appstate;

    if (event->type == SDL_EVENT_QUIT)
    {
        state->running = false;
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        /* Map SDL keys to CHIP-8 keypad (0x0-0xF) */
        switch (event->key.key)
        {
        case SDLK_1:
            state->cpu.keypad[0x1] = 1;
            break;
        case SDLK_2:
            state->cpu.keypad[0x2] = 1;
            break;
        case SDLK_3:
            state->cpu.keypad[0x3] = 1;
            break;
        case SDLK_4:
            state->cpu.keypad[0xC] = 1;
            break;
        case SDLK_Q:
            state->cpu.keypad[0x4] = 1;
            break;
        case SDLK_W:
            state->cpu.keypad[0x5] = 1;
            break;
        case SDLK_E:
            state->cpu.keypad[0x6] = 1;
            break;
        case SDLK_R:
            state->cpu.keypad[0xD] = 1;
            break;
        case SDLK_A:
            state->cpu.keypad[0x7] = 1;
            break;
        case SDLK_S:
            state->cpu.keypad[0x8] = 1;
            break;
        case SDLK_D:
            state->cpu.keypad[0x9] = 1;
            break;
        case SDLK_F:
            state->cpu.keypad[0xE] = 1;
            break;
        case SDLK_Z:
            state->cpu.keypad[0xA] = 1;
            break;
        case SDLK_X:
            state->cpu.keypad[0x0] = 1;
            break;
        case SDLK_C:
            state->cpu.keypad[0xB] = 1;
            break;
        case SDLK_V:
            state->cpu.keypad[0xF] = 1;
            break;
        }
    }

    if (event->type == SDL_EVENT_KEY_UP)
    {
        switch (event->key.key)
        {
        case SDLK_1:
            state->cpu.keypad[0x1] = 0;
            break;
        case SDLK_2:
            state->cpu.keypad[0x2] = 0;
            break;
        case SDLK_3:
            state->cpu.keypad[0x3] = 0;
            break;
        case SDLK_4:
            state->cpu.keypad[0xC] = 0;
            break;
        case SDLK_Q:
            state->cpu.keypad[0x4] = 0;
            break;
        case SDLK_W:
            state->cpu.keypad[0x5] = 0;
            break;
        case SDLK_E:
            state->cpu.keypad[0x6] = 0;
            break;
        case SDLK_R:
            state->cpu.keypad[0xD] = 0;
            break;
        case SDLK_A:
            state->cpu.keypad[0x7] = 0;
            break;
        case SDLK_S:
            state->cpu.keypad[0x8] = 0;
            break;
        case SDLK_D:
            state->cpu.keypad[0x9] = 0;
            break;
        case SDLK_F:
            state->cpu.keypad[0xE] = 0;
            break;
        case SDLK_Z:
            state->cpu.keypad[0xA] = 0;
            break;
        case SDLK_X:
            state->cpu.keypad[0x0] = 0;
            break;
        case SDLK_C:
            state->cpu.keypad[0xB] = 0;
            break;
        case SDLK_V:
            state->cpu.keypad[0xF] = 0;
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

/* Main loop - execute one CPU cycle per frame */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    Chip8Emulator *state = (Chip8Emulator *)appstate;
    static Uint64 last_timer_update = 0;

    if (!state->running)
    {
        return SDL_APP_SUCCESS;
    }

    /* Execute one CPU cycle */
    cpu_cycle(&state->cpu);

    /* Decrement timers at 60 Hz */
    Uint64 current_time = SDL_GetTicks();
    if (last_timer_update == 0)
    {
        last_timer_update = current_time;
    }
    
    if (current_time - last_timer_update >= DECREMENT_TIMER_MS)
    {
        cpu_decrement_timers(&state->cpu);
        last_timer_update = current_time;
    }

    /* Update audio based on sound_timer */
    audio_update(&state->audio, state->cpu.sound_timer);

    /* Update display if draw flag is set */
    if (state->cpu.draw_flag)
    {
        display_draw(&state->display, &state->cpu);
        state->cpu.draw_flag = false;
    }

    /* ~1kHz execution (1ms per cycle) */
    SDL_Delay(1);

    return SDL_APP_CONTINUE;
}

/* Cleanup */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    Chip8Emulator *state = (Chip8Emulator *)appstate;
    if (state)
    {
        display_cleanup(&state->display);
        audio_cleanup(&state->audio);
        SDL_free(state);
    }
}