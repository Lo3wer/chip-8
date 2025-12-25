#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include "cpu.h"

// Display scaling factor (64x32 is too small to see)
#define SCALE_FACTOR 10

// Window dimensions
#define WINDOW_WIDTH (DISPLAY_WIDTH * SCALE_FACTOR)
#define WINDOW_HEIGHT (DISPLAY_HEIGHT * SCALE_FACTOR)

// Display colors (can customize these)
#define COLOR_OFF 0x000000FF  // Black
#define COLOR_ON  0xFFFFFFFF  // White

// Display context structure
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

// Initialize SDL and create window/renderer
bool display_init(Display *display);

// Render the CHIP-8 display buffer to the screen
void display_draw(Display *display, const Cpu *cpu);

// Clean up SDL resources
void display_cleanup(Display *display);

// Clear the screen (optional helper)
void display_clear(Display *display);

#endif