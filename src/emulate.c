#include "chip8.h"
#include "display.h"
#include "sound.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Invalid input\n");
        return 1;
    }

    char* rom = argv[1];

    // Init Chip8
    init_chip8();

    // Load and check ROM file
    load_rom(rom);

    // Init display
    init_screen();

    // Init sound
    init_audio();

    while (1)
    {
        cycle();
        keypad_handler(keypad);
        if (quit_app())
        {
            break;
        }

        if (sound)
        {
            make_sound();
        }

        if (show)
        {
            display_screen(display);
        }

        usleep(1000);
    }

    stop_audio();
    stop_display();
    return 0;
}