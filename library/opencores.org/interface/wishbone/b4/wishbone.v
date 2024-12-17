//-----------------------------------------------------------------------------
// File          : wishbone.v
// Creation date : 12.12.2024
// Creation time : 13:41:34
// Description   : 
// Created by    : 
// Tool : Kactus2 3.13.3 64-bit
// Plugin : Verilog generator 2.4
// This file was generated based on IP-XACT component opencores.org:interface:wishbone:b4
// whose XML file is C:/Users/suokuism/programming/kactus2dev/Library/opencores.org/interface/wishbone/b4/wishbone.b4.xml
//-----------------------------------------------------------------------------

module wishbone(
    // These ports are not in any interface
    input                [7:0]          core_iaddr_o,
    input                [8:0]          core_local_address_o,
    input                [31:0]         core_local_write_data,
    input                               core_local_write_o,
    input  reg           [31:0]         wishbone_bridge_mem_data_out,    // Data to controller.
    input  reg                          wishbone_bridge_mem_slave_rdy,    // Bridge has executed the transfer.
    output               [27:0]         core_instruction_feed,
    output               [31:0]         core_local_read_data,
    output               [15:0]         wishbone_bridge_mem_address_in,    // Target address of a peripheral operation.
    output               [31:0]         wishbone_bridge_mem_data_in,    // Data from controller.
    output                              wishbone_bridge_mem_master_rdy,    // Data is provided and transfer can be executed.
    output                              wishbone_bridge_mem_we_in    // Controllers writes = 1, Controller reads = 0.
);

  localparam AXI_TARGETS = 1;
  localparam AXI_INITIATORS = 1;
  localparam IdWidthInits = 8 + $clog2(AXI_TARGETS);

  AXI_LITE #(
    .AXI_ADDR_WIDTH(32),
    .AXI_DATA_WIDTH(32)
  ) axi4lite_target [AXI_TARGETS-1:0]();

  AXI_LITE #(
    .AXI_ADDR_WIDTH(32),
    .AXI_DATA_WIDTH(32)
  ) axi4lite_init [AXI_INITIATORS-1:0]();

  localparam NoAddrRules = AXI_TARGETS + 0;

  typedef axi_pkg::xbar_rule_32_t rule_t;

  rule_t [NoAddrRules-1:0] AddrMapXBAR;

  assign AddrMapXBAR = 
    '{
      '{idx: 32'd0, start_addr: 32'h0, end_addr: 32'hffff} //core_imem_bridge
    };

  localparam axi_pkg::xbar_cfg_t xbar_cfg = '{
    NoSlvPorts:         AXI_TARGETS,
    NoMstPorts:         AXI_INITIATORS,
    MaxMstTrans:        1,
    MaxSlvTrans:        1,
    FallThrough:        1'b0,
    LatencyMode:        axi_pkg::CUT_ALL_AX,
    AxiIdWidthSlvPorts: 8,
    AxiIdUsedSlvPorts:  0,
    UniqueIds:          1'b1,
    AxiAddrWidth:       32,
    AxiDataWidth:       32,
    NoAddrRules:        NoAddrRules
  };

  axi_lite_xbar_intf #(
    .Cfg(xbar_cfg),
    .rule_t(rule_t)
  ) i_axi_lite_xbar(
    .clk_i(),
    .rst_ni(),
    .test_i(1'b0),
    .slv_ports(axi4lite_init),
    .mst_ports(axi4lite_target),
    .addr_map_i(AddrMapXBAR),
    .en_default_mst_port_i('0),
    .default_mst_port_i('0)
  );



endmodule
