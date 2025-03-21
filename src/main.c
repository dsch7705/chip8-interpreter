#include "chip8.h"
#include <errno.h>
#include <stdio.h>
#include <time.h>

#include "gui.h"


extern int quit;
extern uint8_t gui_quit;

//#define DBG
int main(int argc, char** argv)
{
	int err = 0;

#ifndef DBG
	if (argc != 2)
	{
		printf("[USAGE] chip <rom>\n");
		return -1;
	}

	err = load_rom(argv[1]);
#else
	err = load_rom("roms/rom2.ch8");
#endif // !DBG


	init_cpu();
	if (err != 0)
	{
		if (err == -1)
		{
			perror("[ERR] fread() error\n");
		}
		else
		{
			perror("[ERR] Error loading ROM\n");
		}

		return -1;
	}
	printf("[OK] ROM loaded successfully!\n");
	

	init_gui();
	while (!gui_quit)
	{
		for (int i = 0; i < 11; i++)
		{
			cycle();
		}
		decrement_registers();

		draw_gui();
		handle_gui_events();
	}
}