#include "gui.h"
#include "chip8.h"
#include <raylib/raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raylib/raygui.h>

#include <stdint.h>


uint8_t gui_quit = 0;
uint8_t display_scale = 10;

#define DISPLAY_RECT (Rectangle) { 0, 0, 64 * display_scale, 32 * display_scale + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }
#define REGISTERS_RECT (Rectangle) { 0, DISPLAY_RECT.height, DISPLAY_RECT.width / 5, GetScreenHeight() - DISPLAY_RECT.height }
#define STACK_RECT (Rectangle) { REGISTERS_RECT.x + REGISTERS_RECT.width, REGISTERS_RECT.y, REGISTERS_RECT.width, REGISTERS_RECT.height }
#define MEMORY_RECT (Rectangle) { STACK_RECT.x + STACK_RECT.width, STACK_RECT.y, DISPLAY_RECT.width - STACK_RECT.width * 2, STACK_RECT.height }

void init_gui()
{
	SetTraceLogLevel(LOG_ERROR);
	InitWindow(DISPLAY_RECT.width, 800, "CHIP-8");
	SetTargetFPS(60);

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
		Rectangle r = { REGISTERS_RECT.x + 3.f, REGISTERS_RECT.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + GuiGetStyle(DEFAULT, TEXT_SIZE) * i,
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
		Color c = (i == sp) ? RED : BLACK;

		sprintf(text_buf, "%X:", i);
		Rectangle r = { STACK_RECT.x + 3.f, STACK_RECT.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + GuiGetStyle(DEFAULT, TEXT_SIZE) * i,
			STACK_RECT.width, GuiGetStyle(DEFAULT, TEXT_SIZE) };
		GuiDrawText(text_buf, r, TEXT_ALIGN_LEFT, c);

		sprintf(text_buf, "%03X", stack[i]);
		r.x = STACK_RECT.x + STACK_RECT.width / 2;
		GuiDrawText(text_buf, r, TEXT_ALIGN_LEFT, c);
	}
}

Vector2 mem_scroll = { 0 };
Rectangle mem_view = { 0 };
void draw_memory()
{
	Rectangle content_rect = { MEMORY_RECT.x, MEMORY_RECT.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, MEMORY_RECT.width, GuiGetStyle(DEFAULT, TEXT_SIZE) * sizeof(memory) / 8 };

	char title[32];
	sprintf(title, "Memory	|	PC: %03x", pc);

	GuiScrollPanel(MEMORY_RECT, title, content_rect, &mem_scroll, &mem_view);

	// Scroll to new memory address if program counter changes
	static uint16_t last_pc = 0;
	if (pc != last_pc)
		mem_scroll.y = min(0, (float)pc / sizeof(memory) * -content_rect.height + GuiGetStyle(DEFAULT, TEXT_SIZE) * 5);
	last_pc = pc;
	// Disable x scrolling
	mem_scroll.x = 0;

	BeginScissorMode(mem_view.x, mem_view.y, mem_view.width, mem_view.height);

	int spacing = 40;
	for (int i = 0; i < sizeof(memory) / 8; i++)
	{
		Rectangle row = { content_rect.x + mem_scroll.x + 3.f, content_rect.y + i * GuiGetStyle(DEFAULT, TEXT_SIZE) + mem_scroll.y, content_rect.width, GuiGetStyle(DEFAULT, TEXT_SIZE) };
		char buf[8];
		sprintf(buf, "%03x:", i * 8);
		
		Rectangle r = row;
		r.width = 50;
		GuiDrawText(buf, r, TEXT_ALIGN_LEFT, BLACK);
		r.x += r.width;
		r.width = spacing;

		for (int j = 0; j < 8; j++)
		{
			char buf[8];
			sprintf(buf, "%02x", memory[i * 8 + j]);
			GuiDrawText(buf, r, TEXT_ALIGN_LEFT, BLACK);
			r.x += r.width;
		}
	}

	EndScissorMode();
}

void draw_gui()
{
	BeginDrawing();
	ClearBackground(WHITE);

	draw_display();
	draw_registers();
	draw_stack();
	draw_memory();

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