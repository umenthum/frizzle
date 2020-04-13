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

#include "textmode.h"
#include <iostream>

void vgpu::on_realize()
{
    Gtk::DrawingArea::on_realize();

    m_pix = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24,
                                        screen_width, screen_height);
    m_gc = Cairo::Context::create(m_pix);

    m_gc->set_source_rgb(0.0, 0.0, 0.0);
    m_gc->rectangle(0, 0, screen_width, screen_height);
    m_gc->fill();
};

bool vgpu::on_draw(const Cairo::RefPtr<Cairo::Context>& gc)
{
    draw_chars();
    gc->save();
    gc->set_source(m_pix, 0, 0);
    gc->paint();
    gc->restore();
    draw_chars();

    return true;
}

uint32_t vgpu::color18to24(uint32_t color) {
    uint8_t r = (color >> 16) & 0x3F;
    uint8_t g = (color >>  8) & 0x3F;
    uint8_t b = (color >>  0) & 0x3F;
    return (r << 18) | (g << 10) | (b << 2);
}

void vgpu::draw_chars()
{
    uint32_t(&raw_array)[screen_height][screen_width] = *((uint32_t(*)[screen_height][screen_width])m_pix->get_data());
    for (int i = 0; i < text_rows; i++) {
        for (int j = 0; j < text_cols; j++) {
            uint8_t attribute_byte = text_array[i][2 * j + 1];
            int fg_color_idx = attribute_byte & 0xF;
            int bg_color_idx = (attribute_byte >> 4) & 0x7;
            bool blink = attribute_byte >> 7;
            uint32_t fg_color = color18to24(color_palette[fg_color_idx]);
            uint32_t bg_color = color18to24(color_palette[bg_color_idx]);
            uint8_t character = text_array[i][2 * j];

            for (int x = 0; x < char_rows; x++) {
                uint8_t row_data = font_data[character][x];
                int y;
                for (y = 0; y < (char_cols - 1); y++) {
                    raw_array[(i * char_rows) + x][(j * char_cols) + y] = (row_data >> (7 - y)) & 0x1 ? fg_color : bg_color;
                }
                raw_array[(i * char_rows) + x][(j * char_cols) + y] = ((character >= 0xC0) && (character < 0xE0) && (row_data & 0x1)) ? fg_color : bg_color;
            }
        }
    }
}

