#include "gui.h"
#include "chip8.h"
#include <raylib/raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raylib/raygui.h>

#include <stdint.h>


uint8_t gui_quit = 0;
uint8_t display_scale = 8;

#define DISPLAY_RECT (Rectangle) { 0, 0, 64 * display_scale, 32 * display_scale + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }
#define REGISTERS_RECT (Rectangle) { 0, DISPLAY_RECT.height, DISPLAY_RECT.width / 2, GetScreenHeight() - DISPLAY_RECT.height }
#define STACK_RECT (Rectangle) { REGISTERS_RECT.width, REGISTERS_RECT.y, REGISTERS_RECT.width, REGISTERS_RECT.height }

void init_gui()
{
	SetTraceLogLevel(LOG_ERROR);
	InitWindow(1400, 800, "CHIP-8");

	GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
	GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);
}

void draw_display()
{
	GuiPanel(DISPLAY_RECT, "Display");
	DrawRectangle(DISPLAY_RECT.x, DISPLAY_RECT.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT,
		DISPLAY_RECT.width, DISPLAY_RECT.height - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, BLACK);

	for (uint8_t y = 0; y < 32; y++)
	{
		for (uint8_t x = 0; x < 64; x++)
		{
			if (display[x + y * 64])
			{
				DrawRectangle(x * display_scale, y * display_scale + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT,
					display_scale, display_scale, WHITE);
			}
		}
	}
}

void draw_registers()
{
	GuiPanel(REGISTERS_RECT, "Registers");
	char text_buf[16];
	int spacing = 15;

	for (int i = 0; i < 16; i++)
	{
		sprintf(text_buf, "V%X:", i);
		Rectangle r = { REGISTERS_RECT.x, REGISTERS_RECT.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + GuiGetStyle(DEFAULT, TEXT_SIZE) * i,
			REGISTERS_RECT.width/2, GuiGetStyle(DEFAULT, TEXT_SIZE) };
		GuiDrawText(text_buf, r, TEXT_ALIGN_LEFT, BLACK);

		sprintf(text_buf, "%02X", V[i]);
		r.x = REGISTERS_RECT.x + REGISTERS_RECT.width / 2;
		GuiDrawText(text_buf, r, TEXT_ALIGN_LEFT, BLACK);
	}
}

void draw_stack()
{
	GuiPanel(STACK_RECT, "Stack");
	char text_buf[16];
	for (int i = 0; i < 16; i++)
	{
		sprintf(text_buf, "%X:", i);
		Rectangle r = { STACK_RECT.x, STACK_RECT.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + GuiGetStyle(DEFAULT, TEXT_SIZE) * i,
			STACK_RECT.width, GuiGetStyle(DEFAULT, TEXT_SIZE) };
		GuiDrawText(text_buf, r, TEXT_ALIGN_LEFT, BLACK);

		sprintf(text_buf, "%03X", stack[i]);
		r.x = STACK_RECT.x + STACK_RECT.width / 2;
		GuiDrawText(text_buf, r, TEXT_ALIGN_LEFT, BLACK);
	}
}

void draw_gui()
{
	BeginDrawing();
	ClearBackground(WHITE);

	draw_display();
	draw_registers();
	draw_stack();

	EndDrawing();
}

void stop_gui()
{
	CloseWindow();
}

void handle_gui_events()
{
	if (WindowShouldClose())
		gui_quit = 1;
}