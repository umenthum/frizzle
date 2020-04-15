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

#include "entry.h"

#define N 80
#define K 37 //K < N/2

//void main();

int foo(int *nodes, int elem);
void bar(int *nodes, int a, int b);

void _start() {
    int i,x;
    int nodes[2*N];
    int edges_head[3*N];
    int edges_tail[3*N];

    //https://en.wikipedia.org/wiki/Generalized_Petersen_graph
    for (i = 0; i < N; i++) {
        edges_head[i] = i;
        edges_tail[i] = (i+1) % N;
        edges_head[i + N] = i;
        edges_tail[i + N] = i + N;
        edges_head[i + N + N] = i + N;
        edges_tail[i + N + N] = ((i + K) % N) + N;
    }

    for (i = 0; i < 2*N; i++) {
        nodes[i] = -1;
    }

    //https://en.wikipedia.org/wiki/Kruskal%27s_algorithm
    for (i = (3*N) - 1; i >= 0; i--) {
        if (foo(nodes, edges_head[i]) != foo(nodes, edges_tail[i])) {
            bar(nodes, edges_head[i], edges_tail[i]);
        }
    }
    i = foo(nodes, K);
    x = nodes[i];
}

int foo(int *nodes, int elem){ //disjoint set find
    if (nodes[elem] < 0) {
        return elem;
    } else {
        return foo(nodes, nodes[elem]);
    }
}

//https://courses.engr.illinois.edu/cs225/fa2018/lectures/slides/cs225fa18-32-GraphsIntro-slides.pdf
void bar(int *nodes, int a, int b) { //disjoint set union
    int find_a = foo(nodes, a);
    int find_b = foo(nodes, b);
    if(find_a != find_b){
        if(nodes[find_a] > nodes[find_b]) {
            nodes[find_b] += nodes[find_a];
            nodes[find_a] = find_b;
        }
        else{
            nodes[find_a] += nodes[find_b];
            nodes[find_b] = find_a;
        }
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

int
__divsi3(int a, int b)
{
    const int bits_in_word_m1 = (int)(sizeof(int) * 8) - 1;
    int s_a = a >> bits_in_word_m1;           /* s_a = a < 0 ? -1 : 0 */
    int s_b = b >> bits_in_word_m1;           /* s_b = b < 0 ? -1 : 0 */
    a = (a ^ s_a) - s_a;                         /* negate if s_a == -1 */
    b = (b ^ s_b) - s_b;                         /* negate if s_b == -1 */
    s_a ^= s_b;                                  /* sign of quotient */
    /*
     * On CPUs without unsigned hardware division support,
     *  this calls __udivsi3 (notice the cast to unsigned int).
     * On CPUs with unsigned hardware division support,
     *  this uses the unsigned division instruction.
     */
    return ((unsigned int)a/(unsigned int)b ^ s_a) - s_a;    /* negate if s_a == -1 */
}

int __modsi3(int a, int b)
{
    return a - __divsi3(a, b) * b;
}

unsigned int
__udivsi3(unsigned int n, unsigned int d)
{
    const unsigned n_uword_bits = sizeof(unsigned int) * 8;
    unsigned int q;
    unsigned int r;
    unsigned sr;
    /* special cases */
    if (d == 0)
        return 0; /* ?! */
    if (n == 0)
        return 0;
    sr = __builtin_clz(d) - __builtin_clz(n);
    /* 0 <= sr <= n_uword_bits - 1 or sr large */
    if (sr > n_uword_bits - 1)  /* d > r */
        return 0;
    if (sr == n_uword_bits - 1)  /* d == 1 */
        return n;
    ++sr;
    /* 1 <= sr <= n_uword_bits - 1 */
    /* Not a special case */
    q = n << (n_uword_bits - sr);
    r = n >> sr;
    unsigned int carry = 0;
    for (; sr > 0; --sr)
    {
        /* r:q = ((r:q)  << 1) | carry */
        r = (r << 1) | (q >> (n_uword_bits - 1));
        q = (q << 1) | carry;
        /* carry = 0;
         * if (r.all >= d.all)
         * {
         *      r.all -= d.all;
         *      carry = 1;
         * }
         */
        const int s = (int)(d - r - 1) >> (n_uword_bits - 1);
        carry = s & 1;
        r -= d & s;
    }
    q = (q << 1) | carry;
    return q;
}

int
__clzsi2(int a)
{
    unsigned int x = (unsigned int)a;
    int t = ((x & 0xFFFF0000) == 0) << 4;  /* if (x is small) t = 16 else 0 */
    x >>= 16 - t;      /* x = [0 - 0xFFFF] */
    unsigned int r = t;       /* r = [0, 16] */
    /* return r + clz(x) */
    t = ((x & 0xFF00) == 0) << 3;
    x >>= 8 - t;       /* x = [0 - 0xFF] */
    r += t;            /* r = [0, 8, 16, 24] */
    /* return r + clz(x) */
    t = ((x & 0xF0) == 0) << 2;
    x >>= 4 - t;       /* x = [0 - 0xF] */
    r += t;            /* r = [0, 4, 8, 12, 16, 20, 24, 28] */
    /* return r + clz(x) */
    t = ((x & 0xC) == 0) << 1;
    x >>= 2 - t;       /* x = [0 - 3] */
    r += t;            /* r = [0 - 30] and is even */
    /* return r + clz(x) */
/*     switch (x)
 *     {
 *     case 0:
 *         return r + 2;
 *     case 1:
 *         return r + 1;
 *     case 2:
 *     case 3:
 *         return r;
 *     }
 */
    return r + ((2 - x) & -((x & 2) == 0));
}
