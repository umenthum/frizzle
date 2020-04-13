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

#include "jtag_tlm.h"

jtag_debugger::jtag_debugger(sc_core::sc_module_name name) :
        sc_module(name), m_pulp_debug("m_pulp_debug"),
        m_jtag_server("m_jtag_server") {

    m_pulp_debug.clk_i(clk_i);
    m_pulp_debug.rst_ni(rst_ni);
    m_pulp_debug.testmode_i(testmode_i);
    m_pulp_debug.tck_i(tck);
    m_pulp_debug.tms_i(tms);
    m_pulp_debug.trst_ni(rst_ni);
    m_pulp_debug.td_i(tdi);
    m_pulp_debug.td_o(tdo);
    m_pulp_debug.tdo_oe_o(tdo_oe);
    m_pulp_debug.debug_int(debug_int);
    m_pulp_debug.dmm_stb(dmm_stb);
    m_pulp_debug.dmm_we(dmm_we);
    m_pulp_debug.dmm_resp(dmm_resp);
    m_pulp_debug.dms_stb(dms_stb);
    m_pulp_debug.dms_we(dms_we);
    m_pulp_debug.dmm_mbe(dmm_mbe);
    m_pulp_debug.dms_mbe(dms_mbe);
    m_pulp_debug.dmm_address(dmm_address);
    m_pulp_debug.dms_address(dms_address);
    m_pulp_debug.dmm_rdata(dmm_rdata);
    m_pulp_debug.dms_rdata(dms_rdata);
    m_pulp_debug.dmm_wdata(dmm_wdata);
    m_pulp_debug.dms_wdata(dms_wdata);

    m_jtag_server.clk(clk_i);
    m_jtag_server.rstn(rst_ni);
    m_jtag_server.tck(tck);
    m_jtag_server.tms(tms);
    m_jtag_server.tdo(tdo);
    m_jtag_server.tdi(tdi);
    m_jtag_server.trstn(trstn);

    request_in_flight1 = NULL;
    request_in_flight2 = NULL;

    SC_METHOD(generator_thread);
    sensitive << clk_i.pos();
}

void jtag_debugger::generator_thread() {
    tlm::tlm_generic_payload *transaction_ptr = NULL;
    if (rst_ni) {
        //debug module master -> bus request
        if (dmm_stb) {
            transaction_ptr = new tlm::tlm_generic_payload();
            transaction_ptr->set_data_ptr(new unsigned char[5]);
            if (dmm_we) {
                transaction_ptr->set_command(tlm::TLM_WRITE_COMMAND);
                *((uint32_t*)transaction_ptr->get_data_ptr()) = dmm_wdata;
                transaction_ptr->get_data_ptr()[4] = dmm_mbe.read().to_uint();
            } else {
                transaction_ptr->set_command(tlm::TLM_READ_COMMAND);
            }
            transaction_ptr->set_address(dmm_address);
            transaction_ptr->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
            transaction_ptr->set_data_length(5);
            transaction_ptr->set_streaming_width(5);
            dmm_request_out_port->write(transaction_ptr);
        }

        //bus response -> debug module master
        if (dmm_response_in_port->num_available() > 0) {
            dmm_response_in_port->read(transaction_ptr);
            dmm_resp = 1;
            dmm_rdata = *(uint32_t*)transaction_ptr->get_data_ptr();
            delete transaction_ptr->get_data_ptr();
            transaction_ptr->set_data_ptr(NULL);
            delete transaction_ptr;
            transaction_ptr = NULL;
        } else {
            dmm_resp = 0;
        }

        //debug module slave response -> bus response
        if (request_in_flight2 != NULL) { //check for last clock cycle's 
            transaction_ptr = request_in_flight2;
            if (transaction_ptr->get_command() == tlm::TLM_READ_COMMAND) {
                *((uint32_t*)transaction_ptr->get_data_ptr()) = dms_rdata;
            }
            dms_response_out_port->write(transaction_ptr);
        }

        if (request_in_flight1 != NULL) { //TODO fix this hacky pipelining
            request_in_flight2 = request_in_flight1;
        } else {
            request_in_flight2 = NULL;
        }

        //bus request -> debug module slave
        if (dms_request_in_port->num_available() > 0) {
            dms_request_in_port->read(transaction_ptr);
            request_in_flight1 = transaction_ptr;
            dms_address = transaction_ptr->get_address();
            if (transaction_ptr->get_command() == tlm::TLM_WRITE_COMMAND) {
                dms_stb = 1;
                dms_we = 1;
                dms_mbe = transaction_ptr->get_data_ptr()[4];
                dms_wdata = *(uint32_t*)transaction_ptr->get_data_ptr();
            } else if (transaction_ptr->get_command() == tlm::TLM_READ_COMMAND) {
                dms_stb = 1;
                dms_we = 0;
            }
        } else {
            dms_stb = 0;
            request_in_flight1 = NULL;
        }
    } else {
        //TODO need anything for reset here?
    }
}

jtag_wrapper::jtag_wrapper(sc_core::sc_module_name name) :
        sc_module(name), m_jtag_debugger("m_jtag_debugger"),
        m_initiator("m_initiator")
{
    m_jtag_debugger.clk_i(clk_i);
    m_jtag_debugger.rst_ni(rst_ni);
    m_jtag_debugger.testmode_i(testmode_i);
    m_jtag_debugger.debug_int(debug_int);

    m_jtag_debugger.dmm_request_out_port(dmm_request_fifo);
    m_initiator.request_in_port(dmm_request_fifo);

    m_initiator.response_out_port(dmm_response_fifo);
    m_jtag_debugger.dmm_response_in_port(dmm_response_fifo);

    m_jtag_debugger.dms_request_in_port(dms_request_fifo);
    m_jtag_debugger.dms_response_out_port(dms_response_fifo);

    dms_socket.register_nb_transport_fw(this, &jtag_wrapper::dms_nb_transport_fw);
    m_initiator.initiator_socket(dmm_socket);
    SC_THREAD(target_thread);
}

tlm::tlm_sync_enum jtag_wrapper::dms_nb_transport_fw(tlm::tlm_generic_payload &transaction,
                tlm::tlm_phase &phase, sc_core::sc_time &delay_time) {
    switch (phase) {
        case tlm::BEGIN_REQ:
            dms_request_fifo.write(&transaction);
            return tlm::TLM_COMPLETED;
        case tlm::END_RESP:
            return tlm::TLM_COMPLETED;
        default:
            sc_assert(false);
            break;
    }
}

void jtag_wrapper::target_thread() {
    while (true) {
        tlm::tlm_generic_payload *transaction_ptr = dms_response_fifo.read();
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        tlm::tlm_phase phase = tlm::BEGIN_RESP;
        tlm::tlm_sync_enum status = dms_socket->nb_transport_bw(*transaction_ptr, phase, delay);
    }
}

