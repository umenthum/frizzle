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

#include "vga_tlm.h"
#include <iostream>

vga::vga(sc_core::sc_module_name name) :
        sc_module(name), vga_win(text_array, font_data, color_palette), gtkwin_thread(gtk_run, this) {
    //TODO initialize text_array?
    std::copy(std::begin(default_font_data), std::end(default_font_data), std::begin(font_data));
    std::copy(std::begin(default_color_palette), std::end(default_color_palette), std::begin(color_palette));
    for (int i = 0; i < num_colors; i++) {
        sc_assert(color_palette[i] == default_color_palette[i]);
    }
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < char_rows; j++) {
            sc_assert(font_data[i][j] == default_font_data[i][j]);
        }
    }
    SC_THREAD(target_thread);
}

uint8_t *vga::get_address(uint32_t address) {
    if ((address >= font_base) && (address < font_base + sizeof(font_data))) {
        return &((uint8_t*)font_data)[address-font_base];
    } else if ((address >= text_base) && (address < text_base + sizeof(text_array))) {
        return &((uint8_t*)text_array)[address-text_base];
    } else if ((address >= color_base) && (address < color_base + sizeof(color_palette))) {
        return &((uint8_t*)color_palette)[address-color_base];
    } else {
        sc_assert(false && "bad VGA address");
        return NULL;
    }
}

void vga::gtk_run(vga *vga_obj) {
	Gtk::Main::run(vga_obj->vga_win);
}

void vga::target_thread() {
    while (true) {
        tlm::tlm_generic_payload *transaction_ptr = NULL;
        request_in_port->read(transaction_ptr);

        uint8_t *address = get_address(transaction_ptr->get_address());

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
                vga_win.queue_draw();
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

vga_wrapper::vga_wrapper(sc_core::sc_module_name name) :
        sc_module(name), m_vga("m_vga") {
    target_socket.register_nb_transport_fw(this, &vga_wrapper::nb_transport_fw);
    m_vga.request_in_port(request_fifo);
    m_vga.response_out_port(response_fifo);
    SC_THREAD(target_thread);
}

tlm::tlm_sync_enum vga_wrapper::nb_transport_fw(tlm::tlm_generic_payload &transaction,
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

void vga_wrapper::target_thread() {
    while (true) {
        tlm::tlm_generic_payload *transaction_ptr = response_fifo.read();
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        tlm::tlm_phase phase = tlm::BEGIN_RESP;
        tlm::tlm_sync_enum status = target_socket->nb_transport_bw(*transaction_ptr, phase, delay);
    }
}

