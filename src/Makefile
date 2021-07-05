GCC=gcc
CFLAGS=-Wall -I/usr/local/include
LDFLAGS=-lm -lSDL2 -L/usr/local/lib

all:
	$(GCC) -o chip8 sound.c emulate.c display.c chip8.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f chip8 chipee_tests
