#ifndef INOUT_H
#define INOUT_H

#include <stdint.h>


void init_screen(uint8_t pixel_scale);
void draw();
void handle_events();
void destroy_screen();

void print_registers();

int test;

#endif