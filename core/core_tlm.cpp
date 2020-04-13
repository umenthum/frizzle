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

#include "core_tlm.h"

rv_core::rv_core(sc_core::sc_module_name name) :
        sc_module(name), m_core("core"), request_in_flight(NULL) {

    m_core.clk(clk_i);
    m_core.resetn(rst_ni);
    m_core.debug_irq(debug_int);
    m_core.mem_read(read);
    m_core.mem_write(write);
    m_core.mem_resp(resp);
    m_core.mem_byte_enable(mbe);
    m_core.mem_address(address);
    m_core.mem_rdata(rdata);
    m_core.mem_wdata(wdata);
    m_core.uart_rx_int(uart_rx_int);
    m_core.uart_tx_int(uart_tx_int);
    m_core.uart_rxfifo_int(uart_rxfifo_int);
    m_core.uart_txfifo_int(uart_txfifo_int);

    SC_METHOD(generator_thread);
    sensitive << clk_i.pos();
}

void rv_core::generator_thread() {
    tlm::tlm_generic_payload *transaction_ptr = NULL;
    if (rst_ni) {
        //debug module master -> bus request
        if ((read || write) && (resp == 0) && (request_in_flight == NULL)) {
            transaction_ptr = new tlm::tlm_generic_payload();
            transaction_ptr->set_data_ptr(new unsigned char[5]);
            if (write) {
                transaction_ptr->set_command(tlm::TLM_WRITE_COMMAND);
                *((uint32_t*)transaction_ptr->get_data_ptr()) = wdata;
                transaction_ptr->get_data_ptr()[4] = mbe.read().to_uint();
            } else {
                transaction_ptr->set_command(tlm::TLM_READ_COMMAND);
            }
            transaction_ptr->set_address(address);
            transaction_ptr->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
            transaction_ptr->set_data_length(5);
            transaction_ptr->set_streaming_width(5);
            request_in_flight = transaction_ptr;
            request_out_port->write(transaction_ptr);
        }

        //bus response -> debug module master
        if (response_in_port->num_available() > 0) {
            response_in_port->read(transaction_ptr);
            sc_assert(request_in_flight == transaction_ptr); //one at a time, in order
            resp = 1;
            rdata = *(uint32_t*)transaction_ptr->get_data_ptr();
            delete transaction_ptr->get_data_ptr();
            transaction_ptr->set_data_ptr(NULL);
            delete transaction_ptr;
            transaction_ptr = NULL;
            request_in_flight = NULL;
        } else {
            resp = 0;
        }
    } else {
        //TODO need anything for reset here?
    }
}

rv_core_wrapper::rv_core_wrapper(sc_core::sc_module_name name) :
        sc_module(name), m_core("m_core"),
        m_initiator("m_initiator")
{
    m_core.clk_i(clk_i);
    m_core.rst_ni(rst_ni);
    m_core.debug_int(debug_int);
    m_core.uart_rx_int(uart_rx_int);
    m_core.uart_tx_int(uart_tx_int);
    m_core.uart_rxfifo_int(uart_rxfifo_int);
    m_core.uart_txfifo_int(uart_txfifo_int);

    m_core.request_out_port(request_fifo);
    m_initiator.request_in_port(request_fifo);

    m_initiator.response_out_port(response_fifo);
    m_core.response_in_port(response_fifo);

    m_initiator.initiator_socket(initiator_socket);
}

