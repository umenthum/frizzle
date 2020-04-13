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

#ifndef _CORE_TLM_H_
#define _CORE_TLM_H_

#include "../core_obj_dir/Vcore.h"
#include "systemc.h"
#include "tlm.h"
#include "generic_initiator.h"

SC_MODULE(rv_core)
{
    public:
        SC_CTOR(rv_core);

        sc_in<bool> clk_i;
        sc_in<bool> rst_ni;
        sc_in<bool> debug_int;
        sc_in<bool> uart_rx_int;
        sc_in<bool> uart_tx_int;
        sc_in<bool> uart_rxfifo_int;
        sc_in<bool> uart_txfifo_int;

#if VM_TRACE
        void trace(VerilatedVcdC* tfp, int levels, int options = 0) {
            m_core.trace(tfp, levels, options);
        }
#endif

    private:

        //debug module master bus
        sc_signal<bool>         read;
        sc_signal<bool>         write;
        sc_signal<bool>         resp;
        sc_signal<sc_bv<4> >    mbe;
        sc_signal<vluint32_t>   address;
        sc_signal<vluint32_t>   rdata;
        sc_signal<vluint32_t>   wdata;

        tlm::tlm_generic_payload *request_in_flight;

        Vcore m_core;
        void generator_thread();

    public:
        sc_core::sc_port<sc_core::sc_fifo_out_if<tlm::tlm_generic_payload*> > request_out_port;
        sc_core::sc_port<sc_core::sc_fifo_in_if <tlm::tlm_generic_payload*> > response_in_port;
};

SC_MODULE(rv_core_wrapper)
{
    public:
        SC_CTOR(rv_core_wrapper);

        tlm::tlm_initiator_socket<> initiator_socket;

        sc_in<bool> clk_i;
        sc_in<bool> rst_ni;
        sc_in<bool> debug_int;
        sc_in<bool> uart_rx_int;
        sc_in<bool> uart_tx_int;
        sc_in<bool> uart_rxfifo_int;
        sc_in<bool> uart_txfifo_int;

#if VM_TRACE
        void trace(VerilatedVcdC* tfp, int levels, int options = 0) {
            m_core.trace(tfp, levels, options);
        }
#endif

    private:
        sc_core::sc_fifo<tlm::tlm_generic_payload*> request_fifo;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> response_fifo;

        rv_core m_core;
        generic_initiator m_initiator;
};

#endif // _CORE_TLM_H_

