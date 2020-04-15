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

#ifndef _ENTRY_H
#define _ENTRY_H

//default stack start, can be overridden
#ifndef STACK_START
 #define STACK_START 0x80200000
#endif // STACK_START

#define __STACK_START_HELPER(x) #x
#define __STACK_START(x) __STACK_START_HELPER(x)

void __attribute__((naked)) _entry() {
    asm volatile (

#ifdef ENTRY_BREAK
        "ebreak\n\t"
#endif

        "li sp, " __STACK_START(STACK_START) "\n\t"
        "call _start\n\t"

#ifdef EXIT_BREAK
        "ebreak\n\t"
#endif

#ifdef NO_WFI
"%=:     j %=b"
#else
"%=:     wfi\n\t"
        "j %=b"
#endif

    ::"r"(0));
}

#endif // _ENTRY_H

