# Frizzle RISC-V Testbench

A RISC-V system simulator with VGA, UART, memory, and JTAG debugging, interconnected with SystemC/TLM, designed with operating systems and computer architecture classroom use in mind.

## Usage

### Compile

`sim.sh examples/testvga.c [debug]`

First argument to `sim.sh` is the C test code you want to compile to test.bin which is loaded into memory in main.cpp. The test is also available in elf format in a.out which can be loaded by GDB.

### Run

`./obj_dir/Vfrizzle`

The sim will wait for an OpenOCD debugger to attach, so in another terminal:

`openocd -f tcl/interface/jtag_vpi.cfg`

To attach GDB (`(gdb) ` represents the GDB prompt)

`riscv64-unknown-elf-gdb a.out`
`(gdb) target remote localhost:3333`

To advance past a breakpoint in the test code (`asm ("ebreak");`)

`(gdb) set $dpc += 4`

## Prerequisites

* Verilator
* SystemC (need environment variables SYSTEMC_LIBDIR, SYSTEMC_INCLUDE, maybe also SYSTEMC)
* RISC-V OpenOCD
* RISC-V GNU tools (GCC, objcopy, objdump, GDB)
* gtkmm-3.0, probably some other packages (sorry I haven't tried running on a blank system to know exactly what is needed)

## Feature Wishlist

* Ethernet MAC connected via MII to virtual PHY, connects to host via TAP. Use lwip as the IP stack
* Some sort of storage/block device/virtual SPI flash
* DRAM delay model (dramsim2)
* Virtual USB
* RISC-V proxy kernel support
* RVFI monitor, shadow memory (checks cache read correctness)
* RISC-V compliance tests
* SystemC core instead of Verilog (easier prototyping of advanced functionality like virtual memory, also "golden model" without giving students an RTL solution)
* Plug-and-play support for popular open source cores (serv, rocketchip, picorv32, vexriscv, minerva, ibex)
* Examples of using non-traditional HDLs with Frizzle (chisel, migen, bluespec)
* Some sort of integration or interoperability with FuseSoC and/or Litex (imagine this might just be "add SystemC/TLM mode to Litex" and fold this project into that)
* Better compatibility (Windows, OS X, even distros other than CentOS may need some work)
* Ability to run simulator on a server and interact via web client (including VGA, OpenOCD, GDB interfaces) and/or compile to WebASM - no need for server

## TODO

* Refactor mostly repeated code across the TLM-SystemC adapters
* Better build flow (`make` rules for examples, top level make file instead of `sim.sh`)
* Fix hard coded paths to boot.bin, test.bin in main.cpp
* Document peripheral interfaces (MMIO address layout, etc)
* Better document dependencies (provide `yum install ...packages...` and `apt install ...packages...`, tips for installing SystemC, Verilator, RISC-V tools)
* Many more... `grep TODO . -R`

## LICENSE

Copyright (c) 2020 Kenneth Umenthum

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

