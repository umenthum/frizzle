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

#include "magic_school_bus.h"

magic_school_bus::magic_school_bus(sc_core::sc_module_name name, int _num_initiators, int _num_targets) :
    sc_module(name), num_initiators(_num_targets), num_targets(_num_initiators), // INTENTIONALLY BACKWARDS
    target_sockets(new target_type [num_targets]), initiator_sockets(new initiator_type [num_initiators]),
    initiator_index(0), target_index(0)
{
    //num_initiators and num_targets will represent the number of initiators and targets the bus will
    //be connected to, respectively, therefore they will represent the number of target sockets and
    //initiator sockets, respectively - targets and initiators are backwards when viewed from the bus's
    //perspective
    for (int i = 0; i < num_initiators; i++) {
        initiator_sockets[i].register_nb_transport_bw(this, &magic_school_bus::handle_target_resp, i);
    }

    for (int i = 0; i < num_targets; i++) {
        target_sockets[i].register_nb_transport_fw(this, &magic_school_bus::handle_initiator_req, i);
    }

    addr_map[0] = NULL; //by default all addresses are mapped to no target
}

void magic_school_bus::register_target(tlm::tlm_target_socket<> &target_socket, uint32_t base_addr, uint32_t size){
    sc_assert(target_index < num_initiators);
    initiator_sockets[target_index](target_socket);

    sc_assert(size > 0);

    //check that the base address is not in the middle of another range that would overlap.
    //since lower bound is "less than or equal to", this also checks that if an entry already
    //exists at this address, it must be the end of the range, not the base
    sc_assert(addr_map.lower_bound(base_addr)->second == NULL);

    addr_map[base_addr] = &initiator_sockets[target_index];

    //check that no entry already exists in the address map that would overlap the new range
    uint32_t upper_bound = base_addr + size;
    bool overflow = __builtin_uadd_overflow(base_addr, size, &upper_bound);
    if (overflow) {
        sc_assert(upper_bound == 0);
        // if base_addr + size over flows to exactly 0 then the address range goes from base_addr
        // to 0xFFFFFFFF, so check that lower bound of 0xFFFFFFFF is base address in order to 
        // check for overlap
        sc_assert(addr_map.lower_bound(-1)->first == base_addr);
    } else {
        sc_assert(addr_map.lower_bound(upper_bound)->first == base_addr);
        if (addr_map.find(upper_bound) == addr_map.end()) { //don't remove adjacent range
            addr_map[upper_bound] = NULL;
        }
    }

    target_index++;
}

void magic_school_bus::register_initiator(tlm::tlm_initiator_socket<> &initiator_socket){
    sc_assert(initiator_index < num_targets);
    target_sockets[initiator_index](initiator_socket);

    initiator_index++;
}

tlm::tlm_sync_enum magic_school_bus::handle_initiator_req(int target_id,
        tlm::tlm_generic_payload &transaction, tlm::tlm_phase &phase,
        sc_core::sc_time &delay_time) {
    auto initiator_it = addr_map.lower_bound(transaction.get_address());
    initiator_type *initiator = initiator_it->second;
    sc_assert(initiator != NULL);
    requester_map[&transaction] = target_id;
    return (*initiator)->nb_transport_fw(transaction, phase, delay_time);
}

tlm::tlm_sync_enum magic_school_bus::handle_target_resp(int initiator_id,
        tlm::tlm_generic_payload &transaction, tlm::tlm_phase &phase,
        sc_core::sc_time &delay_time) {
    //sanity check
    sc_assert(addr_map.lower_bound(transaction.get_address())->second == &initiator_sockets[initiator_id]);
    auto target_it = requester_map.find(&transaction);
    sc_assert(target_it != requester_map.end());
    return target_sockets[target_it->second]->nb_transport_bw(transaction, phase, delay_time);
}

