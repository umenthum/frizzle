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

module top (
    clk_i,
    rst_ni,
    testmode_i,

    //JTAG signals
    tck_i,
    tms_i,
    trst_ni,
    td_i,
    td_o,
    tdo_oe_o,

    debug_int, //debug interrupt request

    //debug module -> memory interface
    dmm_stb,
    dmm_we,
    dmm_resp,
    dmm_mbe,
    dmm_address,
    dmm_rdata,
    dmm_wdata,

    //core -> debug module interface
    dms_stb,
    dms_we,
    dms_mbe,
    dms_address,
    dms_rdata,
    dms_wdata

);

    localparam s_offset = 2;
    localparam s_index = 20; //how big is memory module
    localparam s_mask = 2**s_offset;
    localparam s_line = 8*s_mask;
    localparam s_addr = s_offset + s_index;

    input  logic clk_i;
    input  logic rst_ni;
    input  logic testmode_i;
    input  logic tck_i;
    input  logic tms_i;
    input  logic trst_ni;
    input  logic td_i;
    output logic td_o;
    output logic tdo_oe_o;
    output debug_int;

    output logic dmm_stb;
    output logic dmm_we;
    input  logic dmm_resp;
    output logic [s_mask-1:0] dmm_mbe /* verilator sc_bv */;
    output logic [31:0] dmm_address;
    input  logic [s_line-1:0] dmm_rdata;
    output logic [s_line-1:0] dmm_wdata;

    input  logic dms_stb;
    input  logic dms_we;
    input  logic [s_mask-1:0] dms_mbe /* verilator sc_bv */;
    input  logic [31:0] dms_address;
    output logic [s_line-1:0] dms_rdata;
    input  logic [s_line-1:0] dms_wdata;

    logic dmi_rst_n, dmi_req_valid, dmi_req_ready, dmi_resp_ready, dmi_resp_valid;
    dm::dmi_req_t dmi_req;
    dm::dmi_resp_t dmi_resp;

    dmi_jtag #(.IdcodeValue(32'hA0ECE411)) jtag_dtm (
        .clk_i(clk_i),      // DMI Clock
        .rst_ni(rst_ni),     // Asynchronous reset active low
        .testmode_i(testmode_i),

        .dmi_rst_no(dmi_rst_n), // hard reset
        .dmi_req_o(dmi_req),
        .dmi_req_valid_o(dmi_req_valid),
        .dmi_req_ready_i(dmi_req_ready),

        .dmi_resp_i(dmi_resp),
        .dmi_resp_ready_o(dmi_resp_ready),
        .dmi_resp_valid_i(dmi_resp_valid),

        .tck_i(tck_i),    // JTAG test clock pad
        .tms_i(tms_i),    // JTAG test mode select pad
        .trst_ni(trst_ni),  // JTAG test reset pad
        .td_i(td_i),     // JTAG test data input pad
        .td_o(td_o),     // JTAG test data output pad
        .tdo_oe_o(tdo_oe_o)  // Data out output enable
    );

    dm::hartinfo_t hartinfo;
    assign hartinfo.zero1 = 8'h00;
    assign hartinfo.nscratch = 4'h02;
    assign hartinfo.zero0 = 3'h0;
    assign hartinfo.dataaccess = 1'b1;
    assign hartinfo.datasize = dm::DataCount;
    assign hartinfo.dataaddr = dm::DataAddr;

    dm_top debug_module (
        .clk_i(clk_i),       // clock
        .rst_ni(rst_ni),      // asynchronous reset active low, connect PoR here, not the system reset
        .testmode_i(testmode_i),

        .ndmreset_o(),  // non-debug module reset
        .dmactive_o(),  // debug module is active
        .debug_req_o(debug_int), // async debug request
        .unavailable_i(1'b0), // communicate whether the hart is unavailable (e.g.: power down)
        .hartinfo_i(hartinfo),

        .slave_req_i(dms_stb), //strobe
        .slave_we_i(dms_we), //write enable
        .slave_addr_i(dms_address),
        .slave_be_i(dms_mbe), //mem_byte_enable
        .slave_wdata_i(dms_wdata),
        .slave_rdata_o(dms_rdata),
        // dms_* - debug module slave; dmm_* - debug module master
        .master_req_o(dmm_stb), //strobe
        .master_add_o(dmm_address),
        .master_we_o(dmm_we), //write enable
        .master_wdata_o(dmm_wdata),
        .master_be_o(dmm_mbe), //mem_byte_enable
        .master_gnt_i(dmm_resp),
        .master_r_valid_i(dmm_resp),
        .master_r_rdata_i(dmm_rdata),

        // Connection to DTM - compatible to RocketChip Debug Module
        .dmi_rst_ni(dmi_rst_n),
        .dmi_req_valid_i(dmi_req_valid),
        .dmi_req_ready_o(dmi_req_ready),
        .dmi_req_i(dmi_req),

        .dmi_resp_valid_o(dmi_resp_valid),
        .dmi_resp_ready_i(dmi_resp_ready),
        .dmi_resp_o(dmi_resp)
    );

endmodule

