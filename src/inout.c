#include "inout.h"
#include "chip8.h"
#include <raylib/raylib.h>
#include <stdio.h>
#include <stdlib.h>


uint8_t scale;
int quit = 0;

void init_screen(uint8_t pixel_scale)
{
	scale = pixel_scale;
	InitWindow(64 * scale, 32 * scale, "CHIP-8");
}

void draw_display()
{
	ClearBackground(BLACK);

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (display[x + y * 64])
				DrawRectangle(x * scale, y * scale, scale, scale, RAYWHITE);
		}
	}
}

void draw()
{
	BeginDrawing();

	if (display_flag)
		draw_display();

	EndDrawing();
}

void handle_events()
{
	if (WindowShouldClose() || IsKeyDown(KEY_ESCAPE))
		quit = 1;

	if (IsKeyPressed(KEY_SPACE))
	{
		cycle();
		print_registers();
	}
}

void destroy_screen()
{
	CloseWindow();
}

void print_registers()
{
	system("cls");
	for (int i = 0; i < 16; i++)
	{
		printf("V%X: %X   |   stack[%X]: %X\n", i, V[i], i, stack[i]);
	}
}