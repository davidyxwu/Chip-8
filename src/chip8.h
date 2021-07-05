#ifndef _CHIP8_H_
#define _CHIP8_H_
// Memory
extern unsigned char memory[4096];

// Registers: V0-VE are general purpose, VF is carry flag
extern unsigned char V[16];

// Index Register
extern unsigned short I;

// Program counter
extern unsigned short PC;

// Screen contents (64 x 32)
extern unsigned char display[64 * 32];

// Stack and stack pointer
extern unsigned short stack[16];
extern unsigned short SP;

// Keypad
extern unsigned char keypad[16];

// Timers
extern unsigned char delay_timer;
extern unsigned char sound_timer;

// Show on display/play sound flags
extern unsigned int show;
extern unsigned int sound;

// Functions
void init_chip8();
void load_rom(char* filename);
void zero_instructions(unsigned short opcode);
void eight_instructions(unsigned short opcode, unsigned short Vx, unsigned short Vy);
void E_instructions(unsigned short opcode, unsigned short Vx);
void F_instructions(unsigned short opcode, unsigned short Vx);
void cycle();

#endif
