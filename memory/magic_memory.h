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

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include <map>

SC_MODULE(magic_memory)
{
    public:
        SC_CTOR(magic_memory);
        //TODO support for hex formats
        void load_memory(std::string memory_file, uint32_t address);
        sc_core::sc_port<sc_core::sc_fifo_out_if<tlm::tlm_generic_payload*> > response_out_port;
        sc_core::sc_port<sc_core::sc_fifo_in_if <tlm::tlm_generic_payload*> > request_in_port;
    private:
        const uint32_t page_offset_bits = 12;
        const uint32_t page_size = 1 << page_offset_bits;
        const uint32_t page_offset_mask = page_size - 1;
        const uint32_t page_number_mask = ~page_offset_mask;
        std::map<uint32_t, uint8_t*> page_map;
        void response_thread();
        uint8_t* get_or_create_address(uint32_t address);
};

SC_MODULE(memory_wrapper)
{
    public:
        SC_CTOR(memory_wrapper);
        tlm_utils::simple_target_socket<memory_wrapper> target_socket;
        magic_memory* operator->() { return &m_memory;}
    private:
        tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &transaction,
                tlm::tlm_phase &phase, sc_core::sc_time &delay_time);
        void target_thread();
        magic_memory m_memory;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> request_fifo;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> response_fifo;
};

#endif // _MEMORY_H_

