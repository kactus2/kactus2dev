// Not to be used in designs, only for testing purposes
// This is not VHDL but verilog, in any case the tool must not crash


`timescale 1ns/100ps
module i2c_master(
	       clk,
	       rst_an,
	       bitCount,
	       active,
	       read,
	       full,
	       empty,
	       timerZero,
	       arbFail,
	       setArbFail,
	       setNoAck,
	       noAck,
	       start,
	       stop,
	       i2ctrans,
	       enableRx,
	       enableTx,
	       writeRx,
	       readTx,
	       loadShift,
	       toSCL,
	       toSDA,
	       sdaOut_int,
	       count_hold_over,
	       writeTimer,
	       setDataReq,
	       setDone
	       );

`include "i2c_defs.v"

   input  clk;
   input  rst_an;
   input  [3:0] bitCount;	// number of bits in shift register
   input  active;		// the I2C bus is actively being used
   input  read;		// LSB of address byte, read (write bar)
   input  full;		// receive FIFO if full
   input  empty;		// transmit FIFO if empty
   input  timerZero;	// clock divider timer reached zero
   input  arbFail;	// Arbitration Fail bit from status register
   output setArbFail;	// set Arbitration Fail if master-receiver cannot 'no ack'
   output setNoAck;	// set No Acknowledge flag in status register
   input  noAck;		// no acknowledge bit from status register
   input  start;		// send start cond before this byte, txfifo bit 8
   input  stop;		// send stop cond after this byte, txfifo bit 9
   input  [`T_BUSSIZE:0] i2ctrans;		// I2C bus transitions
   output enableRx;	// enables receiver
   output enableTx;	// enables transmitter
   output writeRx;		// write byte to receive FIFO
   output readTx;		// read byte from transmit FIFO
   output loadShift;	// load byte from transmit FIFO into shift reg
   output toSCL;		// value to drive onto clock line
   output toSDA;		// value to drive onto data line
   input  sdaOut_int;		// value on sda line
   input count_hold_over;	// indicate that minimum hold time over
   output [1:0] writeTimer;	// reload clock divider countdown timer
   output setDataReq;	// set data request flag in the status register
   output setDone;		// set status bit saying transaction completed

   wire arbFail;
   wire setArbFail;
   reg [5:0] state;
   reg [5:0] nextState;
   reg enableRx;
   reg enableTx;
   reg writeRx;
   reg readTx;
   reg loadShift;
   reg toSCL;
   reg toSDA;
   reg readReg;
   reg stopReg;
   reg nextStopReg;
   reg nextReadReg;
   reg [1:0] writeTimer;
   reg setNoAck;
   reg setArbFailInt;
   reg setDataReq;
   reg setDone;

   // Combinational logic

   assign setArbFail = setArbFailInt;

   always @ (  state
	       or rst_an 
	       or bitCount 
	       or i2ctrans 
	       or read
	       or full 
	       or timerZero 
	       or setArbFailInt
	       or arbFail
	       or noAck 
	       or active
	       or empty 
	       or start 
	       or stop 
	       or readReg 
	       or stopReg
	       or count_hold_over
	       or sdaOut_int
	       ) begin

      // default values
      nextState	= state;
      enableRx	= 0;
      enableTx	= 0;
      writeRx	= 0;
      readTx	= 0;
      loadShift	= 0;
      toSCL	= 1;
      toSDA	= 1;
      writeTimer   = 2'b00;
      nextReadReg  = readReg;
      nextStopReg  = stopReg;
      setNoAck     = 0;
      setArbFailInt   = arbFail;
      setDataReq   = 0;
      setDone      = 0;

      case (state)
	 ST_M_IDLE : begin
	       // transmit data and bus are available and START flag is set
	       if (!active & !empty & start & timerZero & !`T_STOP) begin
		     nextState = ST_M_START;
		     writeTimer[1] = 1;
		     nextStopReg = stop;
		     nextReadReg = read;
	       end
	       if (`T_STOP) writeTimer[1] = 1;
	    end
	 ST_M_START : begin	// change SCL from 1 to 0 while SDA = 0
	       toSDA = 0;
	       if (timerZero | `T_SCLFALL) begin
		  toSCL       = 0;
		  readTx      = 1;
		  loadShift   = 1;
		  writeTimer[1]  = 1;
		  nextReadReg = read;
		  nextStopReg = stop;
		  if (read)
		     nextState   = ST_M_ADRLO;
		  else
		     nextState   = ST_M_CLKLO;
	       end
	    end
	 ST_M_ADRLO : begin	// wait with SCL = 0;
	       if (!count_hold_over)
	       toSDA     = sdaOut_int;
	       enableTx = 1;
	       toSCL = 0;
	       if (`T_RAWSCL == 1) begin
		  toSDA = `T_LASTSDA;  // hold SDA if SCL is still high
	       end
	       else if (timerZero) begin
		  nextState = ST_M_ADRHI1;
	       end
	    end
	 ST_M_ADRHI1 : begin	// stop driving SCL low, wait till SCL = 1
	       enableTx = 1;
	       if (`T_SCL) begin
		  writeTimer[1] = 1;
		  nextState = ST_M_ADRHI2;
		  if (`T_SDA & bitCount == 8) setNoAck = 1; // missing acknwldg
	       end
	    end
	 ST_M_ADRHI2 : begin	// wait with SCL = 1;
	       enableTx = 1;
	       if (timerZero | `T_SCLFALL) begin
		  writeTimer[0] = 1;
		  nextState = ST_M_ADRLO;
		  if (bitCount == 'h8) begin
		     if (noAck) begin
			nextState = ST_M_STOP1;
			//flushTx   = 1;
		     end
		     else if (stopReg)
			nextState = ST_M_STOP1;
		     else begin
			if (bitCount == 'h8 & empty) begin
			   nextState = ST_M_DATAREQ;
			   setDataReq = 1;
			end
			else begin
			   readTx = 1;
			   loadShift = 1;
			   nextStopReg = stop;
			   nextState = ST_M_CLKLO;
			end
		     end
		  end
	       end
	    end

	 ST_M_DATAREQ : begin
	       //Wait for data if writing and txfifo is empty
	       toSCL = 0;
	       if (!empty) begin
		  writeTimer[0] = 1;
		  nextStopReg = stop;
		  if (start) begin
		     nextState = ST_M_RESTART2; // wait with SCL, SDA = 1;
		  end
		  else begin
		     readTx     = 1;
		     loadShift  = 1;
		     nextState   = ST_M_CLKLO;
		  end
	       end
	    end

	 ST_M_DATAFULL : begin
	       //Wait for some room in the receive fifo
	       toSCL = 0;
	       if (!full) begin
		  writeTimer[0] = 1;
		  writeRx    = 1;
		  readTx     = 1;	// read dummy byte
		  nextState   = ST_M_CLKLO;
	       end
	    end

	 ST_M_CLKLO : begin	// wait with SCL = 0;
	       toSCL = 0;
	       enableRx = readReg;
	       enableTx = !readReg;
	       if (`T_RAWSCL) begin
	          if (readReg) begin
		  toSDA = 1;
		  end	   
		  else begin
		  toSDA = `T_LASTSDA;
		  end
	       end
	       else if (timerZero) begin
		  nextState = ST_M_CLKHI1;
	       end
	       if (!count_hold_over) begin
	         toSDA = sdaOut_int;
	       end
	    end

	 ST_M_CLKHI1 : begin	// stop driving SCL low, wait till SCL = 1
	       enableRx = readReg;
	       enableTx = !readReg;
	       if (`T_SCL) begin
		  writeTimer[1] = 1;
		  nextState = ST_M_CLKHI2;
		  if (`T_SDA & bitCount == 8) setNoAck = 1; // missing acknwldg
	       end
	    end
	 ST_M_CLKHI2 : begin	// wait with SCL = 1;
	       enableRx = readReg;
	       enableTx = !readReg;
	       if (timerZero | `T_SCLFALL) begin
		  writeTimer[0] = 1;
		  nextState = ST_M_CLKLO;
		  if (bitCount == 'h8) begin
		     if (noAck) begin
			nextState = ST_M_STOP1;
			//flushTx   = 1;
		     end
		     else if (stopReg)
			nextState = ST_M_STOP1;
		     else begin
			if (empty) begin
			   nextState = ST_M_DATAREQ;
			   setDataReq = 1;
			end
			else if (full) begin
			    if (stop) begin
			      nextStopReg = stop;
			    end
			    else begin
			      nextState = ST_M_DATAFULL;
			    end
			end
			else begin
			   nextStopReg = stop;
                           if (readReg) begin
                              writeRx = 1;
                              readTx  = 1;
                           end
                           else begin
                              loadShift = 1;
                              if (!stopReg & start)
                                 nextState = ST_M_RESTART1;
                              else
                                 readTx = 1;
                           end
			end
		     end
		  end
		  if (bitCount == 'h7 & readReg & (stopReg | start | empty))
		     nextState = ST_M_LASTRDLO;
	       end
	    end
         ST_M_LASTRDLO : begin  // wait with SCL = 0;
               toSCL = 0;
	       if (!count_hold_over) 
	         toSDA = sdaOut_int;
               if (timerZero & !full) begin
                  if (stopReg | (!empty & start)) begin
                     writeRx = 1;
                     nextState = ST_M_LASTRDHI1;
                  end
                  else if (!empty) begin
                     nextState = ST_M_CLKLO;
                     writeTimer[0] = 1;
                  end
               end

	       if (!stopReg & empty)
		  setDataReq = 1;

               if (bitCount == 'h8 & (stopReg | (!empty & start)))
                  enableRx = 0;
               else
                  enableRx = 1;

               if (~stopReg & empty) writeTimer[0] = 1;

            end
	 ST_M_LASTRDHI1 : begin	// stop driving SCL low, wait till SCL = 1
	       if (`T_SCL) begin
		  writeTimer[1] = 1;
	          if (`T_SDA == 0) begin	//Expected SDA=1
		     setArbFailInt = 1;
		     nextState = ST_M_IDLE;
		     end
		  else begin
		     nextState = ST_M_LASTRDHI2;
		  end
	       end
	    end
	 ST_M_LASTRDHI2 : begin	// wait with SCL = 1;
	       if (timerZero | `T_SCLFALL) begin
		  writeTimer[0] = 1;
		  if (start)
		     nextState = ST_M_RESTART1;
		  if (stopReg)
		     nextState = ST_M_STOP1;
	       end
	    end
	 ST_M_STOP1 : begin	// wait for falling edge of scl
	       toSCL = 0;
	       if (!count_hold_over) 
	         toSDA = sdaOut_int;
	       if (`T_SCLFALL) begin
	       nextState = ST_M_STOP2;
	       end
	    end
	 ST_M_STOP2 : begin	// wait to with SCL = 0 and SDA = 0
	       toSCL = 0;
	       if (!count_hold_over) begin
	         toSDA = sdaOut_int;
	       end
               else begin
	       toSDA = 0;
	       end
	       if (timerZero) begin
		  writeTimer[1] = 1;
		  nextState = ST_M_STOP3;
	       end
	    end
	 ST_M_STOP3 : begin	// change SCL from 0 to 1, while SDA = 0
	       //letting SDA go to 1 while SCL = 1 completes the STOP condition
	       toSDA = 0;
	       if (`T_SCL == 0 ) writeTimer[1] = 1;
	       if (timerZero) begin
		  nextState = ST_M_IDLE;
		  setDone   = 1;
		  writeTimer[1] = 1;
	       end
	    end
	 ST_M_RESTART1 : begin	// SCL = 0 for 1/2 cycle after Ack clk pulse
	       toSCL = 0;
	       if (timerZero) begin
		  writeTimer[1] = 1;
		  nextState = ST_M_RESTART2;
	       end
	    end
	 ST_M_RESTART2 : begin	// wait with SCL = 1, SDA = 1;
	       if (`T_SCL == 0 ) writeTimer[1] = 1;
	       if (timerZero) begin
		  // transmit data and bus are available and START flag is set
		  nextState = ST_M_START;
		  writeTimer[1] = 1;
	       end
	    end
      endcase


      // Override next state if arbitration failure
      if (setArbFailInt) begin
	 nextState  = ST_M_IDLE;
	 writeTimer[0] = 1;
      end
      // Override next state if reset
      if (!rst_an | `T_STOP) begin
	 nextState  = ST_M_IDLE;
	 writeTimer[0] = 1;
      end
   end

   // State register
   always @ (posedge clk) begin
      state    <= #(`tQ) nextState;
      stopReg  <= #(`tQ) nextStopReg;
      readReg  <= #(`tQ) nextReadReg;
   end

endmodule
