`timescale 1ns/100ps
// Not to be used in designs, only for testing purposes
// This is not VHDL but verilog, in any case the tool must not crash


module i2c (
// APB Interface
  pclk,
  presetn,
  paddr,
  psel,
  penable,
  pwrite,
  prdata,
  pwdata,

// Interrupt
  intr,

// I2C Ports
  ip_clk,
  rst_an,
  sclIn,
  sdaIn,
  sclOut,
  sdaOut
);

//=======================
// parameter definitions
//=======================

parameter IP_ID         = 32'h00000D02;   // Full ID

//===========================
// input/output declarations
//===========================

input  pclk;
input  presetn;
input  [11:0]  paddr;
input  psel;
input  penable;
input  pwrite;
input [31:0]  pwdata;
output [31:0]  prdata;

// Interrupt
output  intr;

// I2C Ports
input   ip_clk;
input   rst_an;
input   sclIn;
input   sdaIn;
output  sclOut;
output  sdaOut;

//==============================
// wire and reg declarations
//==============================


   wire  setDone;
   wire  setNoAck;

   reg   sclOut;
   reg   sdaOut;
   reg   sdaOut_int_1d;  // delayed value of sdaOut_int
   wire sclOut_int;
   wire sdaOut_int;

   wire [`T_BUSSIZE:0] i2ctrans;	// bus transition flags



   wire clearShiftTx;	// clear shift register bit counter, from transmit ctrl
   wire clearShiftRx;	// clear shift register bit counter, from receive ctrl
   wire rxShift;
   wire txShift;
   wire active;
   wire nAck;
   wire masterSclOut;
   wire masterSdaOut;
   wire [1:0] masterWriteTimer;
   wire txSdaOut;
   wire timerZero;
   wire [3:0] bitCount;
   wire shiftSerIn;
   wire shiftSerOut;
   wire [7:0] shiftParOut;
   wire [9:0] sts;
   wire [31:0] ctl;
   wire [5:0] clkDividerHi;
   wire [5:0] clkDividerLo;

   wire [9:0] TxData;
   wire [7:0] RxData;

   wire masterEnRx;
   wire masterEnTx;
   wire masterReadTx;
   wire masterLoadShift;
   wire masterSetDataReq;
   wire masterWriteRx;

   wire nResetTx;
   wire RxFull;
   wire RxEmpty;
   wire TxFull;
   wire TxEmpty;
   wire txSetArbFail;
   wire masterSetArbFail;
   wire count_hold_over;

   reg tmp_sync_ip_clk_rst_an;
   reg tmp_sync_pclk_rst_an;

//=============================================================================

//=====================================================
// Sync reset signal to IP clock domains
//=====================================================

// Reset synchronizer, asynchronous assertion, synchronous release
always @ (posedge ip_clk or negedge rst_an or negedge presetn) begin
   if (rst_an == 0 || presetn == 0)
      tmp_sync_ip_clk_rst_an <= #(`tQ) 0;
   else
      tmp_sync_ip_clk_rst_an <= #(`tQ) 1;
end

assign sync_ip_clk_rst_an = tmp_sync_ip_clk_rst_an & !ctl[`CTL_RESET];


// Reset synchronizer, asynchronous assertion, synchronous release
always @ (posedge pclk or negedge rst_an or negedge presetn) begin
   if (rst_an == 0 || presetn == 0)
      tmp_sync_pclk_rst_an <= #(`tQ) 0;
   else
      tmp_sync_pclk_rst_an <= #(`tQ) 1;
end

assign sync_pclk_rst_an = tmp_sync_pclk_rst_an & !ctl[`CTL_RESET];


// Some top-level registers
always @ (posedge ip_clk) begin
   sclOut   <= #(`tQ) masterSclOut;
   sdaOut   <= #(`tQ) txSdaOut & nAck & masterSdaOut;
   sdaOut_int_1d   <= #(`tQ) txSdaOut & nAck & masterSdaOut; // for internal use delay 1 clock
end


//===================================
// I2C bus transition monitor
//===================================
   i2c_transitions u_transitions(
	.clk(ip_clk),
	.rst_an(tmp_sync_ip_clk_rst_an),
	.rawScl(sclIn),
	.rawSda(sdaIn),
	.i2ctrans(i2ctrans)
	);

//===================================
// Receive control block
//===================================
   i2c_rx u_rx(
	.clk(ip_clk),
	.rst_an(sync_ip_clk_rst_an),
	.enable(masterEnRx),
	.bitCount(bitCount),
	.shift(rxShift),
	.clearShift(clearShiftRx),
	.active(active),
	.nAck(nAck),
	.count_hold_over(count_hold_over),
	.sdaOut_int(sdaOut_int_1d),
	.toShiftReg(shiftSerIn),
	.i2ctrans(i2ctrans)
	);

//===================================
// Transmit control block
//===================================
   i2c_tx u_tx(
	 .clk(ip_clk),
	 .rst_an(sync_ip_clk_rst_an),
	 .enable(masterEnTx),
	 .bitCount(bitCount),
	 .sdaOut(txSdaOut),
	 .sdaOut_int(sdaOut_int_1d),
	 .count_hold_over(count_hold_over),
	 .shift(txShift),
	 .clearShift(clearShiftTx),
	 .fromShiftReg(shiftSerOut),
	 .setArbFail(txSetArbFail),
	 .i2ctrans(i2ctrans)
	 );

//===================================
// 8-bit serial shift register
//===================================
   i2c_shift u_shift(
	.clk(ip_clk),
	.rst_an(sync_ip_clk_rst_an),
	.clear(clearShiftTx | clearShiftRx),
	.shift(rxShift | txShift),
	.load(masterLoadShift),
	.count(bitCount),
        .serIn(shiftSerIn),
	.parIn(TxData[7:0]),
	.serOut(shiftSerOut),
	.parOut(shiftParOut)
	);

//===================================
// Clock divider
//===================================
   i2c_clkdiv u_clkdiv(
	.clk(pclk),
	.ip_clk(ip_clk),
	.rst_an(sync_ip_clk_rst_an),
	.writeHi(clkdivhiWrite),
	.writeLo(clkdivloWrite),
	.data(pwdata[5:0]),
	.load(masterWriteTimer),
	.loadValHi(clkDividerHi),
	.loadValLo(clkDividerLo),
	.zero(timerZero)
	);

//===================================
// Status register
//===================================
   i2c_status u_status(
	.clk(ip_clk),
	.rst_an(sync_ip_clk_rst_an),
	.di(pwdata[1:0]),
	.write(statusWrite),
	.RxFull(RxFull),
	.RxEmpty(RxEmpty),
	.TxFull(TxFull),
	.TxEmpty(TxEmpty),
	.setDone(setDone),
	.setNoAck(setNoAck),
	.clearNoAck(txDataWrite),
	.setArbFail(masterSetArbFail | txSetArbFail),
	.active(active),
	.scl(sclIn),
	.sda(sdaIn),
	.status(sts)
	);

//===================================
// Control register
//===================================
   i2c_control u_control(
	.pclk(pclk),
	.rst_an(sync_pclk_rst_an),
	.write(controlWrite),
	.data(pwdata[31:0]),
	.control(ctl)
	);


//===================================
// APB write decoder
//===================================
   i2c_apb_write_decode u_decode(
        .psel(psel),
        .pwrite(pwrite),
        .penable(penable),
        .paddr(paddr),
        .txDataWrite(txDataWrite), 
        .statusWrite(statusWrite),
        .controlWrite(controlWrite),
        .clkdivhiWrite(clkdivhiWrite),
        .clkdivloWrite(clkdivloWrite)
        );

//===================================
// APB read mux
//===================================
   i2c_apb_read_mux u_mux(
	 .dOut(prdata)
	,.psel(psel)
	,.paddr(paddr)
	,.rxData(RxData)
	,.status(sts)
	,.control(ctl)
	,.clkdivhi(clkDividerHi)
	,.clkdivlo(clkDividerLo)
	,.id(IP_ID)
	); 

//===================================
// Master control block
//===================================
   i2c_master u_master(
	.clk(ip_clk),
	.rst_an(sync_ip_clk_rst_an),
	.bitCount(bitCount),
	.active(active),
	.read(TxData[0]),
	.full(RxFull),
	.empty(TxEmpty),
	.timerZero(timerZero),
	.arbFail(sts[`STS_AF]),
	.setArbFail(masterSetArbFail),
	.setNoAck(setNoAck),
	.noAck(sts[`STS_NA]),
	.start(TxData[8]),
	.stop(TxData[9]),
	.i2ctrans(i2ctrans),
	.enableRx(masterEnRx),
	.enableTx(masterEnTx),
	.writeRx(masterWriteRx),
	.readTx(masterReadTx),
	.loadShift(masterLoadShift),
	.toSCL(masterSclOut),
	.toSDA(masterSdaOut),
	.sdaOut_int(sdaOut_int_1d),
	.count_hold_over(count_hold_over),
	.writeTimer(masterWriteTimer),
	.setDataReq(masterSetDataReq),  // not used
	.setDone(setDone)
	);

//===================================
// Receive FIFO
//===================================
   i2c_rxfifo u_rxfifo(
	 .wr_clk(ip_clk),
	 .wr_n(!masterWriteRx),
	 .rd_clk(pclk),
	 .rd_n(psel&!pwrite&penable&(paddr== 0)),   
	 .rst_an(sync_ip_clk_rst_an),
	 .di(shiftParOut),
	 .full(RxFull),
	 .empty(RxEmpty),
	 .do(RxData)
	);

//===================================
// Transmit FIFO
//===================================
   i2c_txfifo u_txfifo(
	 .wr_clk(pclk),
	 .wr_n(!txDataWrite),
	 .rd_clk(ip_clk),
	 .rd_n(!masterReadTx),
	 .rst_an(sync_ip_clk_rst_an & !(masterSetArbFail | txSetArbFail)),
	 .di(pwdata[9:0]),
	 .full(TxFull),
	 .empty(TxEmpty),
	 .do(TxData)
	);


//===================================
// Interrupt control
//===================================
   i2c_intr u_intr(
         .status(sts[9:0]),
         .control(ctl[31:0]),
	 .interrupt(intr)
	 );

//===================================

endmodule
