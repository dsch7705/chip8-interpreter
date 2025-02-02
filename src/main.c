#include "chip8.h"
#include <errno.h>
#include <stdio.h>
#include <time.h>

#include "gui.h"


extern int quit;
extern uint8_t gui_quit;

int main(int argc, char** argv)
{
	//init_screen(16);
	init_cpu();
	int err = load_rom("roms/rom2.ch8");
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

		return 1;
	}
	printf("[OK] ROM loaded successfully!\n");
	

	init_gui();
	while (!gui_quit)
	{
		cycle();
		//print_registers();
		//draw();

		//handle_events();

		draw_gui();
		handle_gui_events();

	}

	//destroy_screen();
}