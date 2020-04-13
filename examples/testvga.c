/*
 * Copyright (c) 2020 Kenneth Umenthum
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <stdint.h>
#include "../vga/vga_common.h"

void draw_test_pattern();
void draw_puskar();

//void __attribute__((section("entry"))) _start() {
void _start() {
    asm ("ebreak");
    draw_test_pattern();
    asm ("ebreak");
    draw_puskar();
    draw_test_pattern();
}

void draw_test_pattern() {
    volatile uint8_t (*text_array)[text_rows][2 * text_cols] = (uint8_t (*)[text_rows][2 * text_cols]) text_base;

    for (uint32_t i = 0; i < text_rows; i++) {
        for (uint32_t j = 0; j < text_cols; j++) {
            (*text_array)[i][2*j] = (i*text_cols) + j;
            (*text_array)[i][2*j+1] = (j % 16) | ((i % 8) << 4);
        }
    }
}

static uint8_t puskar[80][25] = {
    {  2,  2,  2,  2,  3,  3,  3,  3,  2,  2,  3,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  4,  5,  6,  7},
    {  2,  2,  2,  2,  3,  3,  3,  3,  2,  2,  3,  4,  4,  4,  4,  3,  3,  3,  3,  3,  4,  4,  5,  6,  7},
    {  2,  2,  2,  2,  2,  3,  3,  3,  2,  2,  3,  3,  4,  4,  4,  3,  4,  3,  3,  3,  4,  5,  6,  6,  7},
    {  2,  2,  2,  2,  3,  3,  3,  3,  2,  2,  3,  3,  3,  4,  3,  3,  3,  3,  3,  4,  4,  5,  6,  7,  7},
    {  2,  2,  2,  3,  3,  3,  3,  3,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  5,  6,  7,  7},
    {  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  5,  6,  6,  7},
    {  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  3,  3,  3,  3,  3,  3,  3,  4,  5,  6,  6,  6,  6},
    {  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  3,  3,  3,  3,  3,  4,  4,  5,  6,  6,  6,  6},
    {  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  3,  3,  3,  3,  4,  4,  4,  4,  5,  6,  6,  6,  6},
    {  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  3,  4,  4,  5,  6,  6,  6,  7},
    {  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  3,  4,  4,  5,  6,  7,  7,  7},
    {  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  5,  6,  7,  7,  7,  7},
    {  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  5,  6,  7,  7,  7,  7},
    {  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  5,  6,  7,  7,  7,  7,  7},
    {  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  7,  7},
    {  1,  1,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  5,  6,  6,  6,  6,  6,  7,  7},
    {  0,  0,  0,  0,  1,  2,  3,  3,  3,  3,  3,  3,  3,  4,  3,  4,  4,  5,  6,  6,  6,  6,  6,  7,  7},
    {  0,  0,  0,  0,  0,  1,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  6,  6,  6,  6,  6,  7,  7},
    {  0,  0,  0,  0,  0,  0,  0,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  6,  6,  6,  6,  6,  7,  7},
    {  1,  0,  1,  0,  1,  0,  0,  0,  2,  6,  6,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  6,  7,  8},
    {  2,  1,  1,  1,  2,  3,  2,  1,  1,  2,  4,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  9},
    {  2,  2,  2,  3,  6,  7,  7,  8,  8,  7,  7,  5,  5,  9,  6,  5,  6,  6,  6,  6,  6,  6,  7,  8,  9},
    {  1,  2,  3,  7, 10, 10, 10, 10,  8,  6,  8, 10,  9, 10, 12,  8,  6,  6,  6,  6,  6,  7,  7,  8,  9},
    {  0,  2,  6,  9, 11, 11, 11, 11,  7,  7,  8,  9, 11, 11, 11, 11,  6,  6,  6,  6,  6,  7,  8,  8,  9},
    {  1,  3,  8, 11, 12, 12, 12, 12,  8,  8,  5,  7, 11, 13, 12, 11,  8,  6,  6,  6,  7,  8,  8,  8,  9},
    {  1,  5, 10, 13, 13, 13, 13, 13,  8,  7,  6,  9, 10, 13, 13, 13, 11,  8,  6,  6,  7,  8,  8,  9,  9},
    {  2,  7, 12, 14, 14, 14, 14, 13,  8,  6, 10, 12, 10, 13, 14, 13, 12, 10,  8,  7,  7,  8,  8,  8,  9},
    {  2,  8, 13, 15, 15, 14, 14, 13,  7,  4,  4,  9,  9, 13, 14, 14, 13, 11, 10,  8,  7,  7,  8,  8,  9},
    {  3,  8, 14, 15, 15, 15, 15, 14,  8,  3,  1,  7,  9, 13, 15, 14, 13, 12, 11,  9,  8,  7,  7,  8,  8},
    {  3,  8, 14, 15, 15, 15, 15, 14,  9,  3,  5,  8,  9, 14, 15, 14, 13, 12, 11, 10,  9,  7,  7,  7,  7},
    {  2,  8, 14, 15, 15, 15, 15, 14, 10,  4,  7,  8, 11, 14, 15, 14, 12, 12, 10,  9,  9,  8,  7,  7,  7},
    {  2,  8, 14, 14, 15, 14, 14, 14, 11,  6,  7,  8, 13, 14, 14, 12, 11, 10,  9,  7,  9,  9,  7,  8,  8},
    {  3,  8, 14, 14, 14, 14, 15, 13, 11,  8,  9, 10, 13, 13, 13, 11, 12, 12, 11,  7,  9, 10,  8,  7,  8},
    {  4, 10, 14, 14, 14, 14, 15, 14, 12, 12, 11, 12, 12, 12, 12, 12, 12, 12, 11,  6, 10, 10,  9,  8,  9},
    {  6, 11, 13, 13, 13, 14, 15, 13, 12, 12, 13, 13, 13, 13, 13, 14, 12, 11, 12,  5,  8, 10, 10, 10,  7},
    {  7, 11, 13, 13, 13, 13, 14, 13, 11, 10, 11, 12, 13, 14, 14, 15, 13, 10, 12,  9, 10,  9, 10, 11,  9},
    {  8, 11, 12, 12, 13, 13, 14, 12,  9,  6,  6,  7,  8, 10, 11, 12, 12,  7, 11, 10, 11,  9,  9, 12, 10},
    {  8, 11, 12, 12, 12, 13, 13, 11,  6,  4,  4,  6,  6,  6,  8, 10,  8,  4, 10, 10, 12, 10,  7, 12, 10},
    {  7, 10, 11, 12, 12, 12, 12,  9,  3,  3,  5,  6,  7,  6,  6,  7,  6,  3,  9,  8, 11, 10,  6, 12, 10},
    {  6,  9, 10, 11, 11, 11, 11,  7,  3,  4,  7,  8,  8,  7,  6,  6,  5,  4,  8,  8, 12,  9,  5, 12, 10},
    {  5,  8,  9, 10, 10, 11, 11,  5,  3,  8,  9, 10, 10,  9,  6,  6,  5,  5,  8,  7,  9,  6,  6, 12, 10},
    {  5,  7,  9, 10, 10, 10, 10,  4,  4, 13, 12, 11, 11, 11,  9,  6,  4,  5,  7,  5,  8,  5,  7, 11,  9},
    {  4,  6,  8,  9,  9, 10,  9,  4,  5,  6, 11, 10, 12, 12, 11,  9,  7,  6,  6,  4,  6,  6,  8, 11,  8},
    {  4,  6,  8,  9,  9,  9,  7,  5,  3,  2,  8,  9, 12, 13, 13, 11,  9,  6,  5,  3,  6,  6,  9, 10,  7},
    {  3,  5,  7,  8,  8,  8,  5,  5,  4,  5,  7,  9, 12, 13, 13, 12, 10,  7,  4,  6,  8,  7,  9,  9,  5},
    {  2,  5,  6,  7,  7,  7,  4,  5,  4,  7,  6,  9, 12, 13, 12, 11, 10,  7,  7,  8,  8,  7,  8,  7,  4},
    {  2,  3,  4,  5,  6,  7,  5,  5,  3,  4,  5, 10, 12, 12, 12, 11,  9,  8,  8,  8,  7,  6,  7,  6,  3},
    {  1,  2,  3,  4,  5,  5,  4,  4,  4,  4,  7, 10, 11, 12, 11, 10,  8,  7,  7,  6,  5,  5,  5,  4,  3},
    {  0,  1,  2,  2,  4,  4,  4,  4,  4,  6,  8, 10, 11, 10,  9,  8,  7,  6,  5,  5,  5,  5,  4,  3,  2},
    {  0,  0,  1,  1,  3,  4,  4,  4,  5,  7,  8, 10,  9,  9,  7,  6,  5,  5,  4,  4,  5,  4,  3,  2,  2},
    {  0,  0,  0,  1,  2,  4,  5,  5,  5,  7,  7,  8,  8,  7,  6,  5,  5,  5,  5,  4,  4,  3,  3,  2,  2},
    {  0,  0,  0,  0,  1,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  4,  5,  5,  4,  3,  3,  3,  3,  2},
    {  0,  0,  0,  0,  0,  2,  3,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  2},
    {  0,  0,  0,  0,  0,  1,  1,  2,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  2},
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  4,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  3,  2},
    {  0,  0,  0,  0,  0,  0,  1,  1,  1,  2,  5,  4,  4,  4,  4,  4,  4,  4,  3,  4,  4,  4,  4,  2,  2},
    {  0,  0,  0,  0,  0,  1,  4,  3,  4,  4,  2,  5,  5,  4,  4,  4,  3,  3,  3,  3,  3,  3,  2,  2,  5},
    {  0,  0,  0,  0,  0,  3,  5,  5,  3,  2,  4,  5,  5,  6,  6,  6,  4,  1,  2,  3,  2,  1,  2,  5,  6},
    {  0,  0,  0,  0,  2,  4,  5,  6,  3,  3,  5,  5,  5,  6,  6,  6,  5,  0,  0,  0,  0,  2,  5,  6,  7},
    {  2,  3,  3,  3,  4,  3,  2,  3,  4,  4,  4,  5,  6,  6,  6,  6,  6,  0,  0,  0,  2,  4,  6,  7,  7},
    {  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  1,  0,  3,  4,  5,  6,  7,  8},
    {  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  5,  3,  4,  4,  5,  6,  7,  8},
    {  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  5,  4,  4,  5,  7,  8,  9},
    {  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  4,  4,  6,  7,  8,  9},
    {  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  4,  5,  6,  8,  9},
    {  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  7,  7,  7,  6,  4,  6,  7,  8},
    {  2,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  5,  4,  6,  7},
    {  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  3,  5,  6},
    {  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  4,  3,  5},
    {  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  8,  6,  5,  7},
    {  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  8,  7,  6,  7},
    {  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  8,  8,  6,  6},
    {  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  8,  8,  7,  6},
    {  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  7,  5},
    {  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  5},
    {  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  8,  8,  5},
    {  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  7},
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
};

static uint32_t palette_RGB[16] = {
	0x00000300,
	0x00000300,
	0x00000300,
	0x00000300,
	0x00000400,
	0x00000800,
	0x00000C00,
	0x00001400,
	0x00001C00,
	0x00002000,
	0x00002300,
	0x00002700,
	0x00002D00,
	0x00003500,
	0x00003B00,
	0x00003F00
};

void draw_puskar() {
    volatile uint8_t (*text_array)[text_rows][2 * text_cols] = (uint8_t (*)[text_rows][2 * text_cols]) text_base;
    volatile uint32_t (*color_palette)[num_colors] = (uint32_t (*)[num_colors]) color_base;
    uint8_t buffer[text_cols];
    uint32_t random = 159;

    for(int i = 0; i < num_colors; i++) {
        uint32_t tmp = palette_RGB[i];
        palette_RGB[i] = (*color_palette)[i];
        (*color_palette)[i] = tmp;
    }

    for(int i = 0; i < text_rows; i++) {
        for(int j = 0; j < text_cols; j++) {
            //text_array and puskar are opposite row/column major
            (*text_array)[i][(2*j) + 1] = puskar[j][i];
            (*text_array)[i][2*j] = 0;
        }
    }

    for (int x = 0; x < 30; x++) {
        for(int j = 0; j < text_cols; j++) {
            buffer[j] = (*text_array)[0][2*j];
            (*text_array)[0][2*j] = (uint8_t) random;
            random = (random*1103515245) + 12345;
        }
        for(int i = 1; i < text_rows; i++) {
            for(int j = 0; j < text_cols; j++) {
                uint8_t tmp = (*text_array)[i][2*j];
                (*text_array)[i][2*j] = buffer[j];
                buffer[j] = tmp;
            }
        }
    }

    for(int i = 0; i < num_colors; i++) {
        uint32_t tmp = palette_RGB[i];
        palette_RGB[i] = (*color_palette)[i];
        (*color_palette)[i] = tmp;
    }
}

unsigned int
__mulsi3 (unsigned int a, unsigned int b)
{
  unsigned int r = 0;

  while (a)
    {
      if (a & 1)
        r += b;
      a >>= 1;
      b <<= 1;
    }
  return r;
}

