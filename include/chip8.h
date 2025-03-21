#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>


extern uint8_t fontset[80];
extern uint8_t memory[4096];
extern uint16_t stack[16];

// Registers
extern uint8_t V[16];
extern uint16_t I;
extern uint8_t dt;
extern uint8_t st;
extern uint16_t pc;
extern uint8_t sp;

// IO
extern uint8_t display[64 * 32];
extern uint8_t keypad[16];

// Flags
extern uint8_t display_flag;
extern uint8_t sound_flag;

void init_cpu();
int load_rom(const char* filename);
void cycle();
void decrement_registers();

#endif