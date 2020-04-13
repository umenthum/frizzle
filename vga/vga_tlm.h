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

#ifndef _VGA_TLM_H_
#define _VGA_TLM_H_

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "vga_common.h"
#include "textmode.h"
#include <thread>

class vga;

SC_MODULE(vga)
{
    public:
        SC_CTOR(vga);
        sc_core::sc_port<sc_core::sc_fifo_out_if<tlm::tlm_generic_payload*> > response_out_port;
        sc_core::sc_port<sc_core::sc_fifo_in_if <tlm::tlm_generic_payload*> > request_in_port;
    private:
        //taking VGA mem region to be the standard A0000-BFFFF
        //other than that, these numbers are made up arbitrarily for now
        void target_thread();
        static void gtk_run(vga *vga_obj);
        vgpu_window vga_win;
        std::thread gtkwin_thread;
        uint8_t *get_address(uint32_t address);
        uint8_t text_array[text_rows][2 * text_cols];
        uint8_t font_data[256][char_rows];
        uint32_t color_palette[num_colors];
};

SC_MODULE(vga_wrapper)
{
    public:
        SC_CTOR(vga_wrapper);
        tlm_utils::simple_target_socket<vga_wrapper> target_socket;
        vga* operator->() { return &m_vga;}
    private:
        tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &transaction,
                tlm::tlm_phase &phase, sc_core::sc_time &delay_time);
        void target_thread();
        vga m_vga;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> request_fifo;
        sc_core::sc_fifo<tlm::tlm_generic_payload*> response_fifo;
};

#endif // _VGA_TLM_H_

