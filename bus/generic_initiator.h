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

#ifndef _GENERIC_INITIATOR_H_
#define _GENERIC_INITIATOR_H_
#include "systemc.h"
#include "tlm.h"

SC_MODULE(generic_initiator), virtual public tlm::tlm_bw_transport_if<>
{
    public:
        SC_CTOR(generic_initiator);
        tlm::tlm_initiator_socket<> initiator_socket;
        sc_core::sc_port<sc_core::sc_fifo_in_if <tlm::tlm_generic_payload*> > request_in_port;
        sc_core::sc_port<sc_core::sc_fifo_out_if<tlm::tlm_generic_payload*> > response_out_port;

    private:
        void initiator_thread();
        tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &transaction,
                tlm::tlm_phase &phase, sc_core::sc_time &time);

        void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
            sc_assert(false);
        }
};

#endif // _GENERIC_INITIATOR_H_

