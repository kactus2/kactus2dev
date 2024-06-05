//-----------------------------------------------------------------------------
// File          : cpu_example.v
// Creation date : 14.11.2024
// Creation time : 08:37:27
// Description   : 
// Created by    : 
// Tool : Kactus2 3.13.83 64-bit
// Plugin : Verilog generator 2.4
// This file was generated based on IP-XACT component tut.fi:cpu.structure:cpu_example:1.0
// whose XML file is C:/Users/suokuism/programming/kactus2dev/Library/tut.fi/cpu.structure/cpu_example/1.0/cpu_example.1.0.xml
//-----------------------------------------------------------------------------

module cpu_example();

// WARNING: EVERYTHING ON AND ABOVE THIS LINE MAY BE OVERWRITTEN BY KACTUS2!!!
endmodule

  localparam AXI_TARGETS = 2;
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

  localparam NoAddrRules = AXI_TARGETS + 1;

  typedef axi_pkg::xbar_rule_32_t rule_t;

  rule_t [NoAddrRules-1:0] AddrMapXBAR;

  assign AddrMapXBAR = 
    '{
      '{idx: 32'd0, start_addr: 32'h0, end_addr: 32'h1}, //wb_slave_spi_master
      '{idx: 32'd0, start_addr: 32'h2, end_addr: 32'h3}, //wb_slave_spi_master
      '{idx: 32'd1, start_addr: 32'h11, end_addr: 32'h12} //external_mem_large
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
    .clk_i(
    .rst_ni(
    .test_i(1'b0)
    .slv_ports(
    .mst_ports(
    .addr_map_i(
    .en_default_mst_port_i('0)
    .default_mst_port_i('0)
endmodule
