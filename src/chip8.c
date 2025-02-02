#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

#define debug_print(fmt, ...)							\
	do													\
	{													\
		if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__);	\
	} while (0)

const int DEBUG = 0;


uint8_t fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0,	// 0
	0x20, 0x60, 0x20, 0x20, 0x70,	// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,	// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,	// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,	// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,	// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,	// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,	// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,	// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,	// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,	// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,	// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,	// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,	// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,	// E
	0xF0, 0x80, 0xF0, 0x80, 0x80	// F
};
uint8_t memory[4096] = { 0 };
uint16_t stack[16] = { 0 };

// Registers
uint8_t V[16] = { 0 };
uint16_t I = 0;
uint8_t dt = 0;
uint8_t st = 0;
uint16_t pc = 0x200;
uint8_t sp = 0;

// IO
uint8_t display[64 * 32] = { 0 };
uint8_t keypad[16] = { 0 };

// Flags
uint8_t display_flag = 0;
uint8_t sound_flag = 0;

void init_cpu()
{
	srand((unsigned int)time(NULL));
	// Load fontset into memory
	memcpy(memory, fontset, sizeof(fontset));
}

int load_rom(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) return errno;

	struct stat st;
	stat(filename, &st);
	size_t bytes_read = fread(memory + 0x200, 1, sizeof(memory) - 0x200, fp);

	fclose(fp);
	if (bytes_read != st.st_size)
		return -1;

	return 0;
}

void cycle()
{
	display_flag = 0;
	sound_flag = 0;

	uint16_t op = memory[pc] << 8 | memory[pc + 1];
	uint8_t x = (op & 0x0F00) >> 8;
	uint8_t y = (op & 0x00F0) >> 4;
	
	switch (op & 0xF000)
	{
	case 0x0000:
		switch (op & 0x00FF)
		{
		case 0x00E0:	// 0x00E0: Clear the display
			debug_print("[OK] 0x%X: 0x00E0\n", op);

			memset(display, 0, sizeof(display));
			pc += 2;
			break;

		case 0x00EE:	// 0x00EE: Return from subroutine
			debug_print("[OK] 0x%X: 0x00EE\n", op);

			sp--;
			pc = stack[sp];
			pc += 2;
			break;

		default:
			printf("[ERR] Unknown opcode: 0x%X\n", op);
			break;
		}
		break;

	case 0x1000:	// 0x1nnn: Jump to addr nnn
		debug_print("[OK] 0x%X: 0x1nnn\n", op);

		pc = op & 0x0FFF;
		break;

	case 0x2000:	// 0x2nnn: Call subroutine at nnn
		debug_print("[OK] 0x%X: 0x2nnnn\n", op);

		stack[sp] = pc;
		sp++;
		pc = op & 0x0FFF;
		break;

	case 0x3000:	// 0x3xkk: Skip next instruction if Vx == kk
		debug_print("[OK] 0x%X: 0x3xkk\n", op);

		if (V[x] == (op & 0x00FF))
			pc += 2;
		pc += 2;
		break;

	case 0x4000:	// 0x4xkk: Skip next instruction if Vx != kk
		debug_print("[OK] 0x%X: 0x4xkk\n", op);

		if (V[x] != (op & 0x00FF))
			pc += 2;
		pc += 2;
		break;

	case 0x5000:	// 0x5xy0: Skip next instruction if Vx == Vy
		debug_print("[OK] 0x%X: 0x5xy0\n", op);

		if (V[x] == V[y])
			pc += 2;
		pc += 2;
		break;

	case 0x6000:	// 0x6xkk: Set Vx = kk
		debug_print("[OK] 0x%X: 0x6xkk\n", op);

		V[x] = op & 0x00FF;
		pc += 2;
		break;

	case 0x7000:	// 0x7xkk: Set Vx = Vx + kk
		debug_print("[OK] 0x%X: 0x7xkk\n", op);

		V[x] = V[x] + (op & 0x00FF);
		pc += 2;
		break;

	case 0x8000:
		switch (op & 0x000F)
		{
		case 0x0000:	// 0x8xy0: Set Vx = Vy
			debug_print("[OK] 0x%X: 0x8xy0\n", op);

			V[x] = V[y];
			pc += 2;
			break;

		case 0x0001:	// 0x8xy1: Set Vx = Vx | Vy
			debug_print("[OK] 0x%X: 0x8xy1\n", op);

			V[x] = V[x] | V[y];
			pc += 2;
			break;

		case 0x0002:	// 0x8xy2: Set Vx = Vx & Vy
			debug_print("[OK] 0x%X: 0x8xy2\n", op);

			V[x] = V[x] & V[y];
			pc += 2;
			break;

		case 0x0003:	// 0x8xy3: Set Vx = Vx ^ Vy
			debug_print("[OK] 0x%X: 0x8xy3\n", op);

			V[x] = V[x] ^ V[y];;
			pc += 2;
			break;

		case 0x0004:	// 0x8xy4: Set Vx = Vx + Vy, Set VF = carry
			debug_print("[OK] 0x%X: 0x8xy4\n", op);
			
			V[0xF] = (V[x] + V[y] > 0xFF) ? 1 : 0;
			V[x] += V[y];
			pc += 2;
			break;

		case 0x0005:	// 0x8xy5: Set Vx = Vx - Vy, Set VF = NOT borrow
			debug_print("[OK] 0x%X: 0x8xy5\n", op);

			V[0xF] = (V[x] >= V[y]) ? 1 : 0;
			V[x] -= V[y];
			pc += 2;
			break;

		case 0x0006:	// 0x8xy6: Bitshift Vx right, Set VF = (LSB == 1)
			debug_print("[OK] 0x%X: 0x8xy6\n", op);

			V[0xF] = (V[x] & 0x1) ? 1 : 0;
			V[x] = V[x] >> 1;
			pc += 2;
			break;

		case 0x0007:	// 0x8xy7: Set Vx = Vy - Vx, Set VF = NOT borrow
			debug_print("[OK] 0x%X: 0x8xy7\n", op);

			V[0xF] = (V[y] >= V[x]) ? 1 : 0;
			V[x] = V[y] - V[x];
			pc += 2;
			break;

		case 0x000E:	// 0x8xyE: Bitshift Vx left, Set VF = (MSB == 1)
			debug_print("[OK] 0x%X: 0x8xyE\n", op);

			V[0xF] = ((V[x] >> 7) & 0x1) ? 1 : 0;
			V[x] = V[x] << 1;
			pc += 2;
			break;

		default:
			printf("[ERR] Unknown opcode: 0x%X\n", op);
			break;
		}
		break;

	case 0x9000:	// 0x9xy0: Skip next instruction if Vx != Vy
		debug_print("[OK] 0x%X: 0x9xy0\n", op);

		if (V[x] != V[y])
			pc += 2;
		pc += 2;
		break;

	case 0xA000:	// 0xAnnn: Set I = nnn
		debug_print("[OK] 0x%X: 0xAnnn\n", op);

		I = op & 0x0FFF;
		pc += 2;
		break;

	case 0xB000:	// 0xBnnn: Jump to addr nnn + V0
		debug_print("[OK] 0x%X: 0xBnnn\n", op);

		pc = (op & 0x0FFF) + V[0];
		break;

	case 0xC000:	// 0xCxkk: Set Vx = random byte & kk
		debug_print("[OK] 0x%X: 0xCxkk\n", op);

		V[x] = (rand() % 256) & (op & 0x00FF);
		pc += 2;
		break;

	case 0xD000:	// 0xDxyn: Display n-byte sprite starting at addr I at (Vx, Vy), Set VF = collision
		debug_print("[OK] 0x%X: 0xDxyn\n", op);
		display_flag = 1;

		V[0xF] = 0;
		uint8_t height = op & 0x000F;
		for (uint8_t yline = 0; yline < height; yline++)
		{
			uint8_t row = memory[I + yline];
			for (uint8_t xline = 0; xline < 8; xline++)
			{
				if ((row & (0x80 >> xline)) != 0)
				{
					if (display[V[x] + xline + (V[y] + yline) * 64] == 1)
						V[0xF] = 1;


					display[V[x] + xline + (V[y] + yline) * 64] ^= 1;
				}
			}
		}

		pc += 2;
		break;

	case 0xE000:
		switch (op & 0x00FF)
		{
		case 0x009E:	// 0xEx9E: Skip next instruction if key with the value of Vx is pressed
			debug_print("[OK] 0x%X: 0xEx9E\n", op);

			if (keypad[V[x]])
				pc += 2;
			pc += 2;
			break;

		case 0x00A1:	// 0xExA1: Skip next instruction if key with the value of Vx is not pressed
			debug_print("[OK] 0x%X: 0xExA1\n", op);

			if (!keypad[V[x]])
				pc += 2;
			pc += 2;
			break;

		default:
			printf("[ERR] Unknown opcode: 0x%X\n", op);
			break;
		}
		break;

	case 0xF000:
		switch (op & 0x00FF)
		{
		case 0x0007:	// 0xFx07: Set Vx = dt
			debug_print("[OK] 0x%X: 0xFx07\n", op);

			V[x] = dt;
			pc += 2;
			break;

		case 0x000A:	// 0xFx0A: Wait for a key press, store result in Vx (blocking)
			debug_print("[OK] 0x%X: 0xFx0A\n", op);

			for (uint8_t i = 0; i < 16; i++)
			{
				if (keypad[i])
				{
					pc += 2;
					break;
				}
			}
			break;

		case 0x0015:	// 0xFx15: Set dt = Vx
			debug_print("[OK] 0x%X: 0xFx15\n", op);

			dt = V[x];
			pc += 2;
			break;

		case 0x0018:	// 0xFx18: Set st = Vx
			debug_print("[OK] 0x%X: 0xFx18\n", op);

			st = V[x];
			pc += 2;
			break;

		case 0x001E:	// 0xFx1E: Set I = I + Vx
			debug_print("[OK] 0x%X: 0xFx1E\n", op);

			I += V[x];
			pc += 2;
			break;

		case 0x0029:	// 0xFx29: Set I = location of sprite for digit Vx
			debug_print("[OK] 0x%X: 0xFx29\n", op);

			I = V[x] * 5;
			pc += 2;
			break;

		case 0x0033:	// 0xFx33: Store BCD representation of Vx at addr I, I+1, I+2
			debug_print("[OK] 0x%X: 0xFx33\n", op);

			memory[I] = (V[x] % 1000) / 100;
			memory[I + 1] = (V[x] % 100) / 10;
			memory[I + 2] = (V[x] % 10);
			pc += 2;
			break;

		case 0x0055:	// 0xFx55: Store registers V0 -> Vx at addr I -> I + x
			debug_print("[OK] 0x%X: 0xFx55\n", op);

			memcpy(memory + I, V, x);
			pc += 2;
			break;

		case 0x0065:	// 0xFx65: Read memory at addr I -> I + x into registers V0 -> Vx
			debug_print("[OK] 0x%X: 0xFx65\n", op);

			memcpy(V, memory + I, x);
			pc += 2;
			break;

		default:
			printf("[ERR] Unknown opcode: 0x%X\n", op);
			break;
		}
		break;

	default:
		printf("[ERR] Unknown opcode: 0x%X\n", op);
		break;
	}

	if (dt > 0) dt--;
	if (st > 0)
	{
		sound_flag = 1;
		st--;
	}
}