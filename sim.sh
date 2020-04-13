#!/usr/bin/env bash

#TODO debug and opt SystemC lib paths
if [ "$2" == "debug" ]; then
    OPT="-O0 -g"
    TRACE="--trace"
    TRACE_FLAG="-DVM_TRACE=1"
else
    OPT="-O3"
    TRACE=
    TRACE_FLAG="-DVM_TRACE=0"
fi

VERILATOR_FLAGS="-sc --compiler clang --x-initial 0 $TRACE --report-unoptflat"

export CXX=clang++

riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -static -ffunction-sections -mcmodel=medany -ffreestanding -fvisibility=hidden -nostdlib -nostartfiles $OPT -Tscripts/baremetal_link.ld -Wl,--no-relax $1 && \
    (yes | ./scripts/elf2bin.sh a.out test.bin) && \
    (yes | ./scripts/rv_load_memory.sh debugger/riscv-dbg/debug_rom/debug_rom.S debug_rom.hex 4 scripts/debug_rom_link.ld)

verilator $VERILATOR_FLAGS -CFLAGS "$OPT -std=gnu++11 -fPIC" --top-module wbuart -Mdir uart_obj_dir wbuart.v         -Iuart/wbuart32/rtl
make -j 8 -C uart_obj_dir -f Vwbuart.mk

verilator $VERILATOR_FLAGS -CFLAGS "$OPT -std=gnu++11 -fPIC" --top-module core   -Mdir core_obj_dir core.sv          -Icore
make -j 8 -C core_obj_dir -f Vcore.mk

verilator $VERILATOR_FLAGS -CFLAGS "$OPT -std=gnu++11 -fPIC" --top-module top    -Mdir jtag_obj_dir dm_pkg.sv top.sv -Idebugger/riscv-dbg/debug_rom -Idebugger/riscv-dbg/src -Idebugger/common_cells/src -Idebugger
make -j 8 -C jtag_obj_dir -f Vtop.mk

SOURCES="main.cpp core/core_tlm.cpp uart/uart_tlm.cpp uart/wbuart32/bench/cpp/uartsim.cpp debugger/jtag_vpi/jtagSystemC.cpp debugger/jtag_vpi/jtag_common.c debugger/jtag_tlm.cpp bus/generic_initiator.cpp bus/magic_school_bus.cpp memory/magic_memory.cpp vga/vga_common.cpp vga/vga_tlm.cpp vga/textmode.cpp"
LIBS="$PWD/uart_obj_dir/Vwbuart__ALL.a $PWD/core_obj_dir/Vcore__ALL.a $PWD/jtag_obj_dir/Vtop__ALL.a"
INCLUDE="-I$PWD/core_obj_dir -I$PWD/jtag_obj_dir -I$PWD/uart_obj_dir -I$PWD/bus -I$PWD/vga -I$PWD/debugger -I$PWD/core -I$PWD/uart $(pkg-config gtkmm-3.0 --cflags)"
LIBS="$LIBS $(pkg-config gtkmm-3.0 --libs)"

#clang++ $TRACE_FLAG -DVM_COVERAGE=0 -DVM_SC=1 $LIBS $INCLUDE -std=gnu++11 -fPIC -MMD -Wno-sign-compare -Wno-uninitialized -Wno-unused-parameter -Wno-unused-variable -Wno-shadow $OPT -pthread $SOURCES -o sim

verilator --exe -sc frizzle.sv $VERILATOR_FLAGS -CFLAGS "$OPT -std=gnu++11 -fPIC $INCLUDE" -LDFLAGS "$LIBS" $SOURCES
make -j 8 -C obj_dir -f Vfrizzle.mk

