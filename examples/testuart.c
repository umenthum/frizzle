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

void handler() __attribute__((interrupt("machine")));
volatile uint8_t (*text_array)[text_rows][2 * text_cols] = (uint8_t (*)[text_rows][2 * text_cols]) text_base;
volatile uint8_t *uart_in = ((uint8_t*) 0x10008);
volatile uint8_t *uart_out = ((uint8_t*) 0x1000C);

void _start() {
    asm(
        "csrw mtvec, %0\n\t"
        "csrrsi x0, mstatus, 0x8"
      :
      : "r" (handler)
    );
    *uart_out = 0x4C;
    *uart_out = 0x6C;
    *uart_out = 0x5C;
    *uart_out = 0x7C;
    *uart_out = 0x0D;
    *uart_out = 0x0A;
}

void handler() {
    /*
    uint32_t cause;
    asm volatile (
        "csrr %0, mcause"
      : "=r" (cause)
    );
    */
    static int i;
    uint8_t in_byte = *uart_in;
    (*text_array)[0][2*i] = in_byte;
    (*text_array)[0][2*i+1] = 0x2;
    *uart_out = in_byte + 1;
    i++;
}

