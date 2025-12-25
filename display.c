#include <SDL3/SDL.h>

#include "display.h"
#include "cpu.h"

bool display_init(Display *display)
{
    if (!SDL_CreateWindowAndRenderer("Chip-8 Emulator", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &display->window, &display->renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return false;
    }

    /* Create texture for rendering the display buffer */
    display->texture = SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (!display->texture) {
        SDL_Log("Couldn't create texture: %s", SDL_GetError());
        return false;
    }

    return true;
}

void display_draw(Display *display, const Cpu *cpu)
{
    uint32_t *pixels = NULL;
    int pitch = 0;

    /* Lock texture for updating */
    if (!SDL_LockTexture(display->texture, NULL, (void **)&pixels, &pitch)) {
        SDL_Log("Couldn't lock texture: %s", SDL_GetError());
        return;
    }

    /* Copy CHIP-8 display buffer to texture */
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        pixels[i] = cpu->display[i] ? COLOR_ON : COLOR_OFF;
    }

    SDL_UnlockTexture(display->texture);

    /* Clear and render */
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    SDL_RenderClear(display->renderer);
    SDL_RenderTexture(display->renderer, display->texture, NULL, NULL);
    SDL_RenderPresent(display->renderer);
}

void display_clear(Display *display)
{
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    SDL_RenderClear(display->renderer);
    SDL_RenderPresent(display->renderer);
}

void display_cleanup(Display *display)
{
    if (display->texture) {
        SDL_DestroyTexture(display->texture);
    }
    if (display->renderer) {
        SDL_DestroyRenderer(display->renderer);
    }
    if (display->window) {
        SDL_DestroyWindow(display->window);
    }
}