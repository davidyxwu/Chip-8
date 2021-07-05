#include "chip8.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

/*
 * Chip-8 Specifications:
 * http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 *
*/

// Chip-8 font set for rendering
const unsigned char fontset[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};

// Memory
unsigned char memory[4096];

// Registers: V0-VE are general purpose, VF is carry flag
unsigned char V[16];

// Index Register
unsigned short I;

// Program counter
unsigned short PC;

// Screen contents (64 x 32)
unsigned char display[64 * 32];

// Stack and stack pointer
unsigned short stack[16];
unsigned short SP;

// Keypad
unsigned char keypad[16];

// Timers
unsigned char delay_timer;
unsigned char sound_timer;

// Show on display/play sound flags
unsigned int show;
unsigned int sound;

// Initialize chip8 settings
void init_chip8()
{
    // Set chip8 arrays to zero
    memset(memory, 0, 4096);
    memset(V, 0, 16);
    memset(display, 0, 64 * 32);
    memset(stack, 0, 16 * 2);
    memset(keypad, 0, 16);

    // Set chip8 pointers, timers, etc.
    I = 0;
    PC = 0x200;
    SP = 0;
    delay_timer = 0;
    sound_timer = 0;
    show = 0;
    sound = 0;

    // Time seed
    srand((unsigned int) time(NULL));

    // Load fonts
    for (int i = 0; i < 80; i++)
    {
        memory[i] = fontset[i];
    }
}

// Load rom into memory
void load_rom(char* fname)
{
    FILE* fp;
    // Check for valid rom
    if ((fp = fopen(fname, "rb")))
    {
        ;
    }
    else
    {
        printf("ERROR: Loading ROM Error\n");
        return;
    }

    // Get file size
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    // Read file into a buffer
    char* buffer = (char*) malloc((size + 1) * sizeof(char));
    fread(buffer, size, 1, fp);
    fclose(fp);

    // Load buffer into memory
    for (int i = 0; i < size; i++)
    {
        memory[i + 512] = buffer[i];
    }
    free(buffer);
}

// Instructions with no arguments 0x0000 opcode
void zero_instructions(unsigned short opcode)
{
    switch (opcode & 0x00FF)
    {
        case 0x00E0:
            // CLS Clear display
            for (int i = 0; i < 64 * 32; i++)
            {
                display[i] = 0;
            }
            PC += 2;
            break;
        case 0x00EE:
            // RET return from subroutine
            PC = stack[SP];
            SP--;
            PC += 2;
            break;
        default:
            printf("Unknown OP 1\n");
            break;
    }
}

// Instructions with two registers 0x8000 opcode
void eight_instructions(unsigned short opcode, unsigned short Vx, unsigned short Vy)
{
    switch (opcode & 0x000F)
    {
        case 0x0000:
            // 0x8XY0 set V[Vx] = V[Vy]
            V[Vx] = V[Vy];
            PC += 2;
            break;
        case 0x0001:
            // 0x8XY1 set V[Vx] = V[Vy] or V[Vy]
            V[Vx] |= V[Vy];
            PC += 2;
            break;
        case 0x0002:
            // 0x8XY2 set V[Vx] = V[Vx] & V[Vy]
            V[Vx] &= V[Vy];
            PC += 2;
            break;
        case 0x0003:
            // 0x8XY3 set V[Vx] = V[Vx] ^ V[Vy]
            V[Vx] ^= V[Vy];
            PC += 2;
            break;
        case 0x0004:
            // 0x8XY4 set V[Vx] = V[Vx] + V[Vy], set carry
            V[Vx] += V[Vy];
            if (V[Vx] > 0xFF)
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }
            PC += 2;
            break;
        case 0x0005:
            // 0x8XY5 set V[Vx] = V[Vx] - V[Vy], set carry
            if (V[Vx] > V[Vy])
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }
            V[Vx] -= V[Vy];
            PC += 2;
            break;
        case 0x0006:
            // 0x8XY6 set V[Vx] = V[Vx] SHR 1
            V[0xF] = V[Vx] % 2;
            V[Vx] /= 2;
            PC += 2;
            break;
        case 0x0007:
            // 0x8XY7 set V[Vx] = V[Vx] - V[Vy], set carry, not borrow
            if (V[Vx] < V[Vy])
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }
            V[Vx] = V[Vy] - V[Vx];
            PC += 2;
            break;
        case 0x000E:
            // 0x8XYE 0x8XY6 set V[Vx] = V[Vx] SHL 1
            V[0xF] = V[Vx] >> 7;
            V[Vx] *= 2;
            PC += 2;
            break;
        default:
            printf("Unknown OP 2\n");
            break;
    }
}

// Instructions with keypad 0xE000
void E_instructions(unsigned short opcode, unsigned short Vx)
{
    switch (opcode & 0x00FF)
    {
        case 0x009E:
            // 0xEx9E skip instruction if keypad[V[Vx]] is pressed
            if (keypad[V[Vx]])
            {
                PC += 2;
            }
            PC += 2;
            break;
        case 0x00A1:
            // 0xEXa1 skip instruction if keypad[V[Vx]] is not pressed
            if (!keypad[V[Vx]])
            {
                PC += 2;
            }
            PC += 2;
            break;
        default:
            printf("Unknown OP 3\n");
            break;
    }
}

// Instructions with timer 0xF000
void F_instructions(unsigned short opcode, unsigned short Vx)
{
    switch (opcode & 0x00FF)
    {
        case 0x0007:
            // Set V[Vx] to delay timer
            V[Vx] = delay_timer;
            PC += 2;
            break;
        case 0x000A:
            // 0xFX0A wait for keypress and put value in V[Vx]
            for (int i = 0; i < 16; i++)
            {
                if (keypad[i])
                {
                    V[Vx] = i;
                    PC += 2;
                    break;
                }
            }
            break;
        case 0x0015:
            // 0xFX15 set delay timer to be V[Vx]
            delay_timer = V[Vx];
            PC += 2;
            break;
        case 0x0018:
            // 0xFX18 set sound timer to be V[Vx]
            sound_timer = V[Vx];
            PC += 2;
            break;
        case 0x001E:
            // 0xFX1E add I to V[Vx]
            I += V[Vx];
            PC += 2;
            break;
        case 0x0029:
            // 0xFX29 set I to be the location of sprite for digit V[Vx]
            I = V[Vx] * 5;
            PC += 2;
            break;
        case 0x0033:
            // 0xFX33: Store into memory
            memory[I] = V[Vx] / 100;
            memory[I + 1] = V[Vx] % 100 / 10;
            memory[I + 2] = V[Vx] % 10;
            PC += 2;
            break;
        case 0x0055:
            // Store into memory starting at location I
            for (int i = 0; i <= Vx; i++)
            {
                memory[I + i] = V[i];
            }
            PC += 2;
            break;
        case 0x0065:
            // Load into memory starting at location I
            for (int i = 0; i <= Vx; i++)
            {
                V[i] = memory[I + i];
            }
            PC += 2;
            break;
        default:
            printf("Unknown OP 4\n");
            break;
    }
}

// Cycle CPU
void cycle()
{
    unsigned short opcode = memory[PC + 1] | memory[PC] << 8;
    // Registers
    unsigned short Vx = (opcode & 0x0F00) >> 8;
    unsigned short Vy = (opcode & 0x00F0) >> 4;


    // All instructions
    switch (opcode & 0xF000)
    {
        case 0x0000:
            zero_instructions(opcode);
            break;
        case 0x1000:
            // 0x1bbb jump to address bbb
            PC = opcode & 0x0FFF;
            break;
        case 0x2000:
            // 0x2bbb call subroutine at address bbb
            SP++;
            stack[SP] = PC;
            PC = opcode & 0x0FFF;
            break;
        case 0x3000:
            //0x3xbb skip next instruction if V[Vx] = bb
            if (V[Vx] == (opcode & 0x00FF))
            {
                PC += 2;
            }
            PC += 2;
            break;
        case 0x4000:
            // 0x4xbb skip next instruction if V[Vx] != bb
            if (V[Vx] != (opcode & 0x00FF))
            {
                PC += 2;
            }
            PC += 2;
            break;
        case 0x5000:
            // 0x5xy0 skip next instruction if V[Vx] == V[Vy]
            if (V[Vx] == V[Vy])
            {
                PC += 4;
            }
            else
            {
                PC += 2;
            }
            break;
        case 0x6000:
            // 0x6xbb Set V[Vx] = bb
            V[Vx] = opcode & 0x00FF;
            PC += 2;
            break;
        case 0x7000:
            // 0x7xbb Add bb to V[Vx]
            V[Vx] += opcode & 0x00FF;
            PC += 2;
            break;
        case 0x8000:
            eight_instructions(opcode, Vx, Vy);
            break;
        case 0x9000:
            // 0x9XY0 Skip next instruction if V[Vx] != V[Vy]
            if (V[Vx] != V[Vy])
            {
                PC += 2;
            }
            PC += 2;
            break;
        case 0xA000:
            // 0xAbbb set I to the address bbb
            I = opcode & 0x0FFF;
            PC += 2;
            break;
        case 0xB000:
            // 0xBbbb jump to location bbb + V[0]
            PC = (opcode & 0x0FFF) + V[Vx];
            break;
        case 0xC000:
            // 0xCXbb Set V[Vx] = random & bb
            V[Vx] = rand() & (opcode & 0x00FF);
            PC += 2;
            break;
        case 0xD000:
            // Draw a sprite on coordinate (V[Vx], V[Vy])
            show = 1;
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int y = 0; y < height; y++)
            {
                pixel = memory[I + y];
                for (int x = 0; x < 8; x++)
                {
                    if ((pixel & (0x80 >> x)) != 0)
                    {
                        int coord = V[Vx] + x + ((V[Vy] + y) * 64);
                        if (display[coord] == 1)
                        {
                            V[0xF] = 1;
                        }
                        display[coord] ^= 1;
                    }
                }
            }
            PC += 2;
            break;
        case 0xE000:
            E_instructions(opcode, Vx);
            break;
        case 0xF000:
            F_instructions(opcode, Vx);
            break;
        default:
            printf("Unknown OP 5\n");
            break;
    }

    // Delay timer
    if (delay_timer > 0)
    {
        delay_timer--;
    }

    // sound timer
    if (sound_timer > 0)
    {
        sound = 1;
        sound_timer--;
    }
}