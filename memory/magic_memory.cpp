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

#include "magic_memory.h"
#include <iostream>

magic_memory::magic_memory(sc_core::sc_module_name name) :
        sc_module(name) {
    SC_THREAD(response_thread);
}

void magic_memory::response_thread() {
    while (true) {
        tlm::tlm_generic_payload *transaction_ptr = NULL;
        request_in_port->read(transaction_ptr);

        uint8_t *address = get_or_create_address(transaction_ptr->get_address());

        switch (transaction_ptr->get_command()) {
            case tlm::TLM_WRITE_COMMAND: {
                uint32_t wdata = *(uint32_t*)transaction_ptr->get_data_ptr();
                //TODO pull stuff like this out into magic_school_bus shared helper functions
                uint8_t mbe = transaction_ptr->get_data_ptr()[4];
                for (int i = 0; i < 4; i++) {
                    if ( (mbe >> i) & 0x1) {
                        address[i] = (wdata >> (i*8)) & 0xff;
                    }
                }
                break;
             }
            case tlm::TLM_READ_COMMAND: {
                //TODO pull stuff like this out into magic_school_bus shared helper functions
                *((uint32_t*)transaction_ptr->get_data_ptr()) = *((uint32_t*)address);
                break;
            }
            default:
                sc_assert(false);
        }

        response_out_port->write(transaction_ptr);
    }
}

uint8_t* magic_memory::get_or_create_address(uint32_t address) {
    uint32_t page_number = address >> page_offset_bits;
    uint32_t page_offset = address & page_offset_mask; //TODO alignment
    auto page_it = page_map.find(page_number);
    if (page_it == page_map.end()) {
        page_map[page_number] = new uint8_t[page_size];
        //TODO initialization scheme?
    }
    return &page_map[page_number][page_offset];
}

// NOTE: only raw binary format supported - no elf, hex, etc
void magic_memory::load_memory(std::string memory_file, uint32_t address) {
    ifstream file(memory_file);
    sc_assert(file.is_open());
    while (!file.eof()) {

        //don't go past end of page boundary
        uint32_t size = ((address & page_number_mask) + page_size) - address;

        char *dest = (char*)get_or_create_address(address);
        sc_assert(file.good());
        file.read(dest, size);
        address += size;
    }
}

memory_wrapper::memory_wrapper(sc_core::sc_module_name name) :
        sc_module(name), m_memory("m_memory") {
    target_socket.register_nb_transport_fw(this, &memory_wrapper::nb_transport_fw);
    m_memory.request_in_port(request_fifo);
    m_memory.response_out_port(response_fifo);
    SC_THREAD(target_thread);
}

tlm::tlm_sync_enum memory_wrapper::nb_transport_fw(tlm::tlm_generic_payload &transaction,
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

void memory_wrapper::target_thread() {
    while (true) {
        tlm::tlm_generic_payload *transaction_ptr = response_fifo.read();
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        tlm::tlm_phase phase = tlm::BEGIN_RESP;
        tlm::tlm_sync_enum status = target_socket->nb_transport_bw(*transaction_ptr, phase, delay);
    }
}

