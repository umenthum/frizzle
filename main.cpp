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

#include <systemc.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <verilated_vcd_sc.h>
#include "debugger/jtag_tlm.h"
#include "memory/magic_memory.h"
#include "core/core_tlm.h"
#include "vga/vga_tlm.h"
#include "uart/uart_tlm.h"
#include <gtkmm.h>

vluint64_t main_time = 0;

int sc_main(int argc, char* argv[]) {
	Gtk::Main	main_instance(argc, argv);

    Verilated::debug(0);
    Verilated::randReset(2);
    std::string bin_name;

    if (argc > 1) {
        bin_name = std::string(argv[1]);
        Verilated::commandArgs(argc - 1, argv + 1);
    } else {
        bin_name = "test.bin";
        Verilated::commandArgs(argc, argv);
    }

    ios::sync_with_stdio();

    jtag_wrapper debugger("debugger");
    rv_core_wrapper core("core");
    memory_wrapper memory("memory");
    vga_wrapper vga("vga");
    uart_wrapper uart("uart");
    magic_school_bus bus("bus", 2, 4);

    memory->load_memory(bin_name, 0x80000000);

    bus.register_initiator(core.initiator_socket);
    bus.register_initiator(debugger.dmm_socket);
    bus.register_target(uart.target_socket, 0x10000, 16);
    bus.register_target(memory.target_socket, 0x80000000, 0x80000000);
    bus.register_target(debugger.dms_socket, 0x100, 0xF00);
    bus.register_target(vga.target_socket, 0xA0000, 0x20000);

    sc_clock clk("clk", 2, SC_NS);//, 0.5, 0.0, SC_NS);
    core.clk_i(clk);
    debugger.clk_i(clk);
    uart.clk_i(clk);

    sc_signal<bool> rstn;
    core.rst_ni(rstn);
    debugger.rst_ni(rstn);
    uart.rst_ni(rstn);

    sc_signal<bool> testmode;
    debugger.testmode_i(testmode);
    testmode = 0;

    sc_signal<bool> debug_int;
    core.debug_int(debug_int);
    debugger.debug_int(debug_int);

    sc_signal<bool> uart_rx_int;
    sc_signal<bool> uart_tx_int;
    sc_signal<bool> uart_rxfifo_int;
    sc_signal<bool> uart_txfifo_int;
    uart.uart_rx_int(uart_rx_int);
    uart.uart_tx_int(uart_tx_int);
    uart.uart_rxfifo_int(uart_rxfifo_int);
    uart.uart_txfifo_int(uart_txfifo_int);
    core.uart_rx_int(uart_rx_int);
    core.uart_tx_int(uart_tx_int);
    core.uart_rxfifo_int(uart_rxfifo_int);
    core.uart_txfifo_int(uart_txfifo_int);

    VerilatedVcdSc* tfp = NULL;
#if VM_TRACE
    Verilated::traceEverOn(true);
    sc_start(1,SC_NS);
    tfp = new VerilatedVcdSc;
    debugger.trace(tfp, 99);
    core.trace(tfp, 99);
    uart.trace(tfp, 99);
    tfp->open("vlt_dump.vcd");
#endif

    while (!Verilated::gotFinish()) {
        if (tfp) tfp->flush();
        main_time++;

        if (main_time == 10) rstn = 1;

        sc_start(1,SC_NS);

        if (tfp) tfp->dump(main_time);
    }

    if (tfp) tfp->close(); tfp = NULL;

    return 0;
}

