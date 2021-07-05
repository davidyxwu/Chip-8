#include <SDL2/SDL.h>

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

void init_screen();
void display_screen(unsigned char* display);
void keypad_handler(unsigned char* keys);
int quit_app();
void stop_display();

#endif