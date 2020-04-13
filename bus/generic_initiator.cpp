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

#include "generic_initiator.h"

generic_initiator::generic_initiator(sc_core::sc_module_name name) :
    sc_module(name), initiator_socket("initiator_socket")
{
    initiator_socket(*this);
    SC_THREAD(initiator_thread);
}

void generic_initiator::initiator_thread() {
    while (true) {
        tlm::tlm_generic_payload *transaction_ptr = request_in_port->read();
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        tlm::tlm_phase phase = tlm::BEGIN_REQ;
        tlm::tlm_sync_enum status = initiator_socket->nb_transport_fw(*transaction_ptr, phase, delay);
    }
}

tlm::tlm_sync_enum generic_initiator::nb_transport_bw(tlm::tlm_generic_payload &transaction,
        tlm::tlm_phase &phase, sc_core::sc_time &time) {
    sc_assert(phase == tlm::BEGIN_RESP);
    response_out_port->write(&transaction);
    return tlm::TLM_COMPLETED;
}

