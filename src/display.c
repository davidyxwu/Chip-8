#include <SDL2/SDL.h>
#include "display.h"

SDL_Window* screen;
SDL_Renderer* renderer;

SDL_Scancode keys[16] =
    {
        SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
        SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
        SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
        SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
    };
int quit = 0;

void init_screen()
{
    SDL_Init(SDL_INIT_VIDEO);

    screen = SDL_CreateWindow
    (
        "Chip-8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        64 * 8,
        32 * 8,
        0
    );
    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
}

void display_screen(unsigned char* display)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 32; y++)
        {
            if (display[x + (y * 64)])
            {
                SDL_Rect rectangle;
                rectangle.x = x * 8;
                rectangle.y = y * 8;
                rectangle.w = 8;
                rectangle.h = 8;
                SDL_RenderFillRect(renderer, &rectangle);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void keypad_handler(unsigned char* keys)
{
    SDL_Event event;

    if (SDL_PollEvent(&event))
    {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        switch(event.type)
        {
            case SDL_QUIT:
                quit = 1;
                break;
            default:
                if (state[SDL_SCANCODE_ESCAPE])
                {
                    quit = 1;
                }

                for (int key = 0; key < 16; key++)
                {
                    keys[key] = state[keys[key]];
                }
                break;
        }
    }
}

int quit_app()
{
    return quit;
}

void stop_display()
{
    SDL_DestroyWindow(screen);
    SDL_Quit();
}