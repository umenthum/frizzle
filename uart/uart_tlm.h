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

#ifndef _UART_TLM_H_
#define _UART_TLM_H_

#include "../uart_obj_dir/Vwbuart.h"
#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "wbuart32/bench/cpp/uartsim.h"

SC_MODULE(inverter) {
    sc_in <bool> in;
    sc_out<bool> out;
    void run() {
        bool tmp = in;
        out = !tmp;
    }
    SC_CTOR(inverter) {
        SC_METHOD(run);
        sensitive << in;
    }
};

SC_MODULE(uart)
{
    public:
        SC_CTOR(uart);
        sc_core::sc_port<sc_core::sc_fifo_out_if<tlm::tlm_generic_payload*> > response_out_port;
        sc_core::sc_port<sc_core::sc_fifo_in_if <tlm::tlm_generic_payload*> > request_in_port;
        sc_in<bool> clk_i;
        sc_in<bool> rst_ni;
        sc_out<bool> uart_rx_int;
        sc_out<bool> uart_tx_int;
        sc_out<bool> uart_rxfifo_int;
        sc_out<bool> uart_txfifo_int;

#if VM_TRACE
        void trace(VerilatedVcdC* tfp, int levels, int options = 0) {
            wbuart.trace(tfp, levels, options);
        }
#endif

    private:
        const uint32_t base_addr = 0x10000;

        tlm::tlm_generic_payload *transaction_ptr;

        void target_thread();

        Vwbuart wbuart;
        inverter rst_inv;
        UARTSIM uart_driver;

        sc_signal<bool> rst;

        //wishbone interface
        sc_signal<bool> wb_cyc;
        sc_signal<bool> wb_stb;
        sc_signal<bool> wb_we;
        sc_signal<bool> wb_stall;
        sc_signal<bool> wb_ack;
        sc_signal<sc_bv<2> > wb_addr;
        sc_signal<sc_bv<4> > wb_sel;
        sc_signal<vluint32_t> wb_wdata;
        sc_signal<vluint32_t> wb_rdata;

        //UART
        sc_signal<bool> uart_rx;
        sc_signal<bool> uart_tx;

        //unused UART
        sc_signal<bool> cts_n;
        sc_signal<bool> rts_n;
};

SC_MODULE(uart_wrapper)
{
    public:
        SC_CTOR(uart_wrapper);
        tlm_utils::simple_target_socket<uart_wrapper> target_socket;
        sc_in<bool> clk_i;
        sc_in<bool> rst_ni;
        sc_out<bool> uart_rx_int;
        sc_out<bool> uart_tx_int;
        sc_out<bool> uart_rxfifo_int;
        sc_out<bool> uart_txfifo_int;

#if VM_TRACE
        void trace(VerilatedVcdC* tfp, int levels, int options = 0) {
            m_uart.trace(tfp, levels, options);
        }
#endif

    private:
        tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &transaction,
                tlm::tlm_phase &phase, sc_core::sc_time &delay_time);
        void target_thread();
        uart m_uart;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> request_fifo;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> response_fifo;
};

#endif // _UART_TLM_H_

