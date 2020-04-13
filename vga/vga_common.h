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

#ifndef _FONT_DATA_H
#define _FONT_DATA_H

#include <stdint.h>

//display parameters
//TODO make text rows/cols configurable via MMIO

#define text_rows 25
#define text_cols 80
#define char_rows 16
#define char_cols 9
#define num_colors 16

const int screen_height = text_rows * char_rows;
const int screen_width = text_cols * char_cols;

const uint32_t text_base  = 0xB0000;
const uint32_t  font_base = 0xA0000;
const uint32_t color_base = 0xA8000;

extern uint8_t default_font_data[256][char_rows];
extern uint32_t default_color_palette[num_colors];

#endif // _FONT_DATA_H

