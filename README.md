# Frizzle RISC-V Testbench

A RISC-V system simulator with VGA, UART, memory, and JTAG debugging, interconnected with SystemC/TLM, designed with operating systems and computer architecture classroom use in mind.

## Usage

### Download

`git clone git@github.com:umenthum/frizzle.git`
`cd frizzle`
`git submodule update`

### Compile

`./sim.sh examples/testvga.c [debug]`

First argument to `sim.sh` is the C test code you want to compile to test.bin which is loaded into memory in main.cpp. The test is also available in elf format in a.out which can be loaded by GDB.

### Run

`./obj_dir/Vfrizzle`

The sim will wait for an OpenOCD debugger to attach, so in another terminal:

`openocd -f $OPENOCD_ROOT/tcl/interface/jtag_vpi.cfg`

To attach GDB (`(gdb) ` represents the GDB prompt)

`riscv64-unknown-elf-gdb a.out`

`(gdb) target remote localhost:3333`

To advance past a breakpoint in the test code (`asm ("ebreak");`)

`(gdb) set $dpc += 4`

To get something closer to raw keyboard/UART IO, create a pair of pseudo-terminals (pty):

`socat -d -d pty,raw,echo=0 pty,raw,echo=0`

This command takes your terminal hostage so in another terminal, run frizzle connected to one end of the new pty. socat will print out the names, suppose they are /dev/pts/3 and /dev/pts/4:

`./obj_dir/Vfrizzle < /dev/pts/3 > /dev/pts/3`

And in yet another terminal, attach to the other end with your terminal emulator of choice:

`screen /dev/pts/4`

## Prerequisites

* A RISC-V core! Frizzle was developed around a RISC-V core derived from the UIUC ECE411 course staff's solution to MP1 so I don't feel comfortable sharing it. Ideally the core should support handling debug and UART interrupt requests as well as a handful of related CSRs. You will likely need to modify core/core_tlm.h and core/core_tlm.cpp to work with your core.
* [Verilator](https://www.veripool.org/projects/verilator/wiki/Installing)
  * Recommend installing from source
* [SystemC](https://github.com/accellera-official/systemc)
  * Verilator needs to see environment variables SYSTEMC_LIBDIR, SYSTEMC_INCLUDE, maybe also SYSTEMC
  * Also LD_LIBRARY_PATH needs to include $SYSTEMC_LIBDIR
  * In my experience, both Verilator and SystemC need to be compiled with `CXXFLAGS='-std=c++11'` to work together
* [RISC-V GNU toolchain](https://github.com/riscv/riscv-gnu-toolchain) (GCC, objcopy, objdump, GDB, need to set RISCV environment variable)
* [RISC-V OpenOCD](https://github.com/riscv/riscv-openocd)
  * configure with `--enable-jtag-vpi`
* Centos: gtkmm-3.0, Ubuntu: libgtkmm-3.0-dev
* You will need to add the following to $OPENOCD_ROOT/tcl/interface/jtag_vpi.cfg (device ID can be configured in debugger/top.sv):

`jtag newtap riscv cpu -irlen 5 -expected-id 0xA0ECE411`

`target create riscv.cpu riscv -chain-position riscv.cpu`

`riscv.cpu configure -work-area-phys 0x80000000 -work-area-size 10000 -work-area-backup 1`


## Feature Wishlist

* Ethernet MAC connected via MII to virtual PHY, connects to host via TAP. Use lwip or smoltcp as the IP stack
* Some sort of storage/block device/virtual SPI flash
* DRAM delay model (dramsim2), maybe some simplified version of the SDRAM bus for educational purposes
* Virtual USB
* RISC-V proxy kernel support
* RVFI monitor, shadow memory (checks cache read correctness)
* RISC-V compliance tests
* SystemC core instead of Verilog (easier prototyping of advanced functionality like virtual memory, also "golden model" without giving students an RTL solution)
* Plug-and-play support for popular open source cores (serv, rocketchip, picorv32, vexriscv, minerva, ibex, ariane)
* Examples of using non-traditional HDLs with Frizzle (chisel, migen, bluespec)
* Some sort of integration or interoperability with FuseSoC and/or Litex (imagine this might just be "add SystemC/TLM mode to Litex" and fold this project into that)
* Better compatibility (Windows, OS X, even distros other than CentOS may need some work)
* Ability to run simulator on a server and interact via web client (including VGA, OpenOCD, GDB interfaces) and/or compile to WebASM - no need for server
* Programmable RTC
* Interrupt controller
* Demonstrate interaction with GEM5 via TLM

## TODO

* Refactor mostly repeated code across the TLM-SystemC adapters
* Better build flow (`make` rules for examples, top level make file instead of `sim.sh`)
* Fix hard coded paths to boot.bin, test.bin in main.cpp
* Document peripheral interfaces (MMIO address layout, etc)
* Better document dependencies (provide `yum install ...packages...` and `apt install ...packages...`, tips for installing SystemC, Verilator, RISC-V tools)
* Provide an option for the OpenOCD/JTAG VPI connection initiation to be non-blocking so sims can proceed without having to attach OpenOCD each time (maybe spawn a thread)
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

