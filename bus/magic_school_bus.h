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

#ifndef _MAGIC_SCHOOL_BUS_H_
#define _MAGIC_SCHOOL_BUS_H_

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

SC_MODULE(magic_school_bus)
{
    public:

        SC_HAS_PROCESS(magic_school_bus);
        magic_school_bus(sc_core::sc_module_name name, int _num_initiators, int _num_targets);

        void register_target(tlm::tlm_target_socket<> &target_socket, uint32_t base_addr, uint32_t size);
        void register_initiator(tlm::tlm_initiator_socket<> &target_socket);

    private:

        tlm::tlm_sync_enum handle_initiator_req(int initiator_id, tlm::tlm_generic_payload &transaction,
                tlm::tlm_phase &phase, sc_core::sc_time &delay_time);
        tlm::tlm_sync_enum handle_target_resp(int target_id, tlm::tlm_generic_payload &transaction,
                tlm::tlm_phase &phase, sc_core::sc_time &delay_time);

        typedef tlm_utils::simple_target_socket_tagged<magic_school_bus> target_type;
        typedef tlm_utils::simple_initiator_socket_tagged<magic_school_bus> initiator_type;

        const int num_initiators;
        const int num_targets;
        int initiator_index;
        int target_index;
        target_type *target_sockets;
        initiator_type *initiator_sockets;
        std::map<uint32_t, initiator_type*, std::greater<uint32_t> > addr_map;
        std::map<tlm::tlm_generic_payload*, uint32_t> requester_map;
};

#endif // _MAGIC_SCHOOL_BUS_H_

