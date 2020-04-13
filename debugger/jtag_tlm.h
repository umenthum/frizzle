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

#ifndef _STUB_H_
#define _STUB_H_

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

#include "Vtop.h"
#include "jtag_vpi/jtagSystemC.h"
#include "../bus/generic_initiator.h"
#include "../bus/magic_school_bus.h"

SC_MODULE(jtag_debugger)
{
    public:
        SC_CTOR(jtag_debugger);

        sc_in <bool> clk_i;
        sc_in <bool> rst_ni;
        sc_in <bool> testmode_i;
        sc_out<bool> debug_int;

#if VM_TRACE
        void trace(VerilatedVcdC* tfp, int levels, int options = 0) {
            m_pulp_debug.trace(tfp, levels, options);
        }
#endif

    private:
        //jtag
        sc_signal<bool>         tck;
        sc_signal<bool>         tms;
        sc_signal<bool>         trstn;
        sc_signal<bool>         tdi;
        sc_signal<bool>         tdo;
        sc_signal<bool>         tdo_oe;

        //debug module master bus
        sc_signal<bool>         dmm_stb;
        sc_signal<bool>         dmm_we;
        sc_signal<bool>         dmm_resp;
        sc_signal<sc_bv<4> >    dmm_mbe;
        sc_signal<vluint32_t>   dmm_address;
        sc_signal<vluint32_t>   dmm_rdata;
        sc_signal<vluint32_t>   dmm_wdata;

        //debug module slave bus
        sc_signal<bool>         dms_stb;
        sc_signal<bool>         dms_we;
        sc_signal<sc_bv<4> >    dms_mbe;
        sc_signal<vluint32_t>   dms_address;
        sc_signal<vluint32_t>   dms_rdata;
        sc_signal<vluint32_t>   dms_wdata;

        Vtop m_pulp_debug;
        SystemCJtagServer m_jtag_server;
        void generator_thread();

        tlm::tlm_generic_payload *request_in_flight1;
        tlm::tlm_generic_payload *request_in_flight2;
    public:
        sc_core::sc_port<sc_core::sc_fifo_out_if<tlm::tlm_generic_payload*> > dmm_request_out_port;
        sc_core::sc_port<sc_core::sc_fifo_in_if <tlm::tlm_generic_payload*> > dmm_response_in_port;
        sc_core::sc_port<sc_core::sc_fifo_out_if<tlm::tlm_generic_payload*> > dms_response_out_port;
        sc_core::sc_port<sc_core::sc_fifo_in_if <tlm::tlm_generic_payload*> > dms_request_in_port;
};

SC_MODULE(jtag_wrapper)
{
    public:
        SC_CTOR(jtag_wrapper);

        sc_in <bool> clk_i;
        sc_in <bool> rst_ni;
        sc_in <bool> testmode_i;
        sc_out<bool> debug_int;

        tlm::tlm_initiator_socket<> dmm_socket;
        tlm_utils::simple_target_socket<jtag_wrapper> dms_socket;

#if VM_TRACE
        void trace(VerilatedVcdC* tfp, int levels, int options = 0) {
            m_jtag_debugger.trace(tfp, levels, options);
        }
#endif

    private:
        tlm::tlm_sync_enum dms_nb_transport_fw(tlm::tlm_generic_payload &transaction,
                tlm::tlm_phase &phase, sc_core::sc_time &delay_time);
        void target_thread();

        sc_core::sc_fifo<tlm::tlm_generic_payload*> dmm_request_fifo;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> dmm_response_fifo;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> dms_request_fifo;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> dms_response_fifo;

        generic_initiator m_initiator;
        jtag_debugger m_jtag_debugger;
};

#endif // _STUB_H_

