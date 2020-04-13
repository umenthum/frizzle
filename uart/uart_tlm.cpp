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

#include "uart_tlm.h"
#include <iostream>

uart::uart(sc_core::sc_module_name name) :
        sc_module(name), wbuart("wbuart"), rst_inv("rst_inv"), uart_driver(0), transaction_ptr(NULL) {
    //clocks and resets
    wbuart.i_clk(clk_i);
    wbuart.i_rst(rst);

    //UART IO
    wbuart.i_uart_rx(uart_rx);
    wbuart.o_uart_tx(uart_tx);

    //unused UART IO
    wbuart.i_cts_n(cts_n);
    wbuart.o_rts_n(rts_n);

    //unused defaults
    cts_n = 0;
    wb_cyc = 1;

    //wishbone interface
    wbuart.i_wb_cyc(wb_cyc);
    wbuart.i_wb_stb(wb_stb);
    wbuart.i_wb_we(wb_we);
    wbuart.o_wb_stall(wb_stall);
    wbuart.o_wb_ack(wb_ack);
    wbuart.i_wb_addr(wb_addr);
    wbuart.i_wb_sel(wb_sel);
    wbuart.i_wb_data(wb_wdata);
    wbuart.o_wb_data(wb_rdata);

    //various interrupt signals to CPU
    wbuart.o_uart_rx_int(uart_rx_int);
    wbuart.o_uart_tx_int(uart_tx_int);
    wbuart.o_uart_rxfifo_int(uart_rxfifo_int);
    wbuart.o_uart_txfifo_int(uart_txfifo_int);

    //wbuart.i_rst is active high, incoming rst_ni is active low
    rst_inv.in(rst_ni);
    rst_inv.out(rst);

    uart_driver.setup(25);

    SC_METHOD(target_thread);
    sensitive << clk_i.pos();
}

void uart::target_thread() {

    uart_rx = uart_driver(uart_tx);
    if (rst_ni) {
        if (transaction_ptr && wb_ack) { //check for last clock cycle's 
            if (transaction_ptr->get_command() == tlm::TLM_READ_COMMAND) {
                *((uint32_t*)transaction_ptr->get_data_ptr()) = wb_rdata;
            }
            response_out_port->write(transaction_ptr);
            transaction_ptr = NULL;
        }

        if ((transaction_ptr == NULL) && (request_in_port->num_available() > 0)) {
            request_in_port->read(transaction_ptr);
            uint32_t address = (transaction_ptr->get_address() - base_addr) >> 2;
            sc_assert(address < 4 && "bad uart address");
            wb_addr = address;
            if (transaction_ptr->get_command() == tlm::TLM_WRITE_COMMAND) {
                wb_stb = 1;
                wb_we = 1;
                wb_sel = transaction_ptr->get_data_ptr()[4];
                wb_wdata = *(uint32_t*)transaction_ptr->get_data_ptr();
            } else if (transaction_ptr->get_command() == tlm::TLM_READ_COMMAND) {
                wb_stb = 1;
                wb_we = 0;
            }
        } else {
            wb_stb = 0;
        }
    } else {
        //TODO need anything for reset here?
    }
}

uart_wrapper::uart_wrapper(sc_core::sc_module_name name) :
        sc_module(name), m_uart("m_uart") {

    m_uart.clk_i(clk_i);
    m_uart.rst_ni(rst_ni);

    m_uart.uart_rx_int(uart_rx_int);
    m_uart.uart_tx_int(uart_tx_int);
    m_uart.uart_rxfifo_int(uart_rxfifo_int);
    m_uart.uart_txfifo_int(uart_txfifo_int);

    target_socket.register_nb_transport_fw(this, &uart_wrapper::nb_transport_fw);

    m_uart.request_in_port(request_fifo);
    m_uart.response_out_port(response_fifo);

    SC_THREAD(target_thread);
}

tlm::tlm_sync_enum uart_wrapper::nb_transport_fw(tlm::tlm_generic_payload &transaction,
                tlm::tlm_phase &phase, sc_core::sc_time &delay_time) {
    switch (phase) {
        case tlm::BEGIN_REQ:
            request_fifo.write(&transaction);
            return tlm::TLM_COMPLETED;
        default:
            sc_assert(false);
            break;
    }
}

void uart_wrapper::target_thread() {
    while (true) {
        tlm::tlm_generic_payload *transaction_ptr = response_fifo.read();
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        tlm::tlm_phase phase = tlm::BEGIN_RESP;
        tlm::tlm_sync_enum status = target_socket->nb_transport_bw(*transaction_ptr, phase, delay);
    }
}

