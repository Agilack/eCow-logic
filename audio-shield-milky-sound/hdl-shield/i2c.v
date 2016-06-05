//
// This file may be distributed and/or modified under the terms of the
// GNU General Public License version 2 as published by the Free Software
// Foundation. (See COPYING.GPL for details.)
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDIN_SG THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//

module i2c(

  input clk,
  input clk_100k,
  //i2c master port
  inout i2c_sda_io,
  output i2c_scl,
  input [7:0] DIN,
  output [7:0] DOUT,
  input reset,
  input copy_enable,
  //
  output [7:0] i2c_state
);

// -- Debug - GSG ----------
wire sda_din;
wire sda_dout;
wire sda_oe;

  SB_IO #(
    .PIN_TYPE(6'b1010_01),
    .PULLUP  ( 1'b1     )
  ) iobuf_sda (
    .PACKAGE_PIN  (i2c_sda_io),
    .OUTPUT_ENABLE(sda_oe),
    .D_OUT_0      (sda_dout),
    .D_IN_0       (sda_din)
  );

// assign sda_oe  = 1'b1;
// assign sda_din = 1'b0;

// -----------------


localparam STATE_IDLE=0;
localparam STATE_START=1;
localparam STATE_ADDR=2;
localparam STATE_RW=3;
localparam STATE_ACK_ADDR=4;
localparam STATE_DATA=5;
localparam STATE_ACK_DATA=6;
localparam STATE_STOP=7;
localparam STATE_ACK_ADDR2=8;
localparam STATE_REG=9;
localparam STATE_ACK_REG=10;
localparam STATE_READ=11;
localparam STATE_REPEAT_START=12;
localparam STATE_ADDR2=13;
localparam STATE_RW2=14;
localparam STATE_MASTER_ACK=15;
// localparam STATE_DELAY=16;
// reg [7:0] ID;


reg SDA;
reg SCL;
reg [7:0] DIN_S;
reg [7:0] DOUT_S=8'h55;//0b0011000 is the TLV320DAC3203 device address the LSB is the Read/Write Bit



always @(clk_100k)
begin
	DIN_S [7:0]<=DIN [7:0];
end
assign 	DOUT[7:0]=DOUT_S [7:0];

// reg [23:0] buffer = 24'd0;
reg [6:0] address;
reg [7:0] register;
reg [7:0] data;
reg [1:0] Part=2'b00;
reg [7:0] state=STATE_IDLE;
reg [2:0] counter; 
reg [4:0] delay =5'b00000;
reg [1:0] ID =2'b00;
//
assign i2c_state[7:0] = state [7:0]; ///Debug normally state
reg Read_Write;
reg stateMachineOn;
// wire sda_read;


// Pilot the output enable of the pin SDA
assign sda_oe =((state==STATE_IDLE)||(state==STATE_START)||(state==STATE_STOP)||
				(state==STATE_REPEAT_START)||(state==STATE_ADDR)||(state==STATE_ADDR2)||
				(state==STATE_REG)||(state==STATE_DATA)||(state==STATE_RW)||(state==STATE_RW2)||
				(state==STATE_MASTER_ACK))  ? 1'b1 : 1'b0;

//Put i2c_sda in high impedance when read is needed
assign sda_dout=((state==STATE_IDLE)||(state==STATE_START)||(state==STATE_STOP)||(state==STATE_REPEAT_START)) ? SDA :
				//((state==STATE_ACK_ADDR)||(state==STATE_ACK_ADDR2)||(state==STATE_ACK_DATA)||(state==STATE_ACK_REG)||(state==STATE_READ))? 1'bz :
				((state==STATE_ADDR)||(state==STATE_ADDR2)) ? address[counter] : 
				(state==STATE_REG) ? register[counter]:
				(state==STATE_DATA) ? data[counter]:
				(state ==STATE_RW) ? 1'b0 :
				(state==STATE_RW2) ? Read_Write:
				(state==STATE_MASTER_ACK) ? 1'b1: 
				1'bz; //SDA
				
// assign sda_read=((state==STATE_ACK_ADDR)||(state==STATE_ACK_ADDR2)||(state==STATE_ACK_DATA)||(state==STATE_ACK_REG)||(state==STATE_READ)) ? i2c_sda : 1'bx;
// assign sda_din=((state==STATE_ACK_ADDR)||(state==STATE_ACK_ADDR2)||(state==STATE_ACK_DATA)||(state==STATE_ACK_REG)||(state==STATE_READ)) ? i2c_sda : 1'bx;

assign i2c_scl = ((reset==1)||(state==STATE_IDLE)||(state==STATE_STOP)||
					(state==STATE_START)||(state==STATE_REPEAT_START)) ? SCL : clk_100k;

always @(posedge copy_enable)// or posedge clk_100k) CON
begin
	if (reset==1)begin
		// state<=STATE_IDLE;
		SDA<=1;
		SCL<=1;
		Part<=2'b00;
		address<=7'd0;
		data<=8'd0;
		register<=8'd0;
		counter<=3'd0;
		stateMachineOn<=1'b0;
	end
	
	if (state==STATE_STOP)
	begin
		stateMachineOn<=0;
	end
	
	case(Part)
		0:begin
			address[6:0] = DIN[7:1];
			Read_Write = DIN[0];
			Part=Part+2'b01;			// 7 address bits + Read/Write bit
		end
		1:begin 
			register[7:0] = DIN[7:0];
			if (Read_Write==1) 
			begin
				stateMachineOn=1;
				Part=2'b00;
			end
			else 
			begin
				Part=Part+2'b01;	
			end
		end
		2:begin
			data[7:0] = DIN[7:0];
			stateMachineOn = 1;
			Part<=2'b00;
			end
	endcase
end
// always @(posedge copy_enable)
	// begin
	    // case(Part)
	    // 0:begin
			// #3 address[6:0] = DIN[7:1];
			// #2 Read_Write = DIN[0];
			// Part=Part+2'b01;			// 7 address bits + Read/Write bit
		// end
	    // 1:begin 
			// #5 register[7:0] = DIN[7:0];
			// if (Read_Write==1) 
			// begin
				// stateMachineOn=1;
				// Part=2'b00;
			// end
			// else 
			// begin
				// Part=Part+2'b01;	
			// end
		// end
	    // 2:begin
			// #5 data[7:0] = DIN[7:0];
			// stateMachineOn = 1;
			// Part<=2'b00;
			// end
	    // endcase
	// end
	// create delays for


always @(posedge clk)
begin
	if ((state!=STATE_START)||(state!= STATE_REPEAT_START)||(state!=STATE_STOP))
	begin ID =0;
	end 
	
	if (state==STATE_START && ID !=2'b01)
	begin
		if (delay[4:0]==5'd0)
		begin
			SDA<=0;
			delay<=delay+5'b001;
		end
		else if (delay[4:0]==5'b01111)
		begin
			SCL<=0;
			delay<=0;
			ID=2'b01;// #1 state=STATE_ADDR;
		end
		else begin
			delay<=delay+5'b00001;
		end
	end
	
	if  (state==STATE_REPEAT_START&& ID !=2'b10)
	begin
		if (delay[2:0]==5'd0)
		begin
			SCL=0;
			delay<=delay+5'b001;
		end
		 else if (delay[2:0]==5'b00110)
		begin
			SDA=1;
			delay<=delay+5'b001;
		end
		else if (delay[2:0]==5'b01100) begin
			SCL=1;
			delay<=delay+5'b001;
		 end
		 else if (delay[4:0]==5'b10010) begin
			SDA=0; 
			delay<=delay+5'b001;
		 end
		 else if (delay[4:0]==5'b11000) begin
			SCL=0;
			ID =2'b10;
			delay<=5'd0;
		 end
		 else begin
			delay<=delay+5'b001;
		end
		 // state=STATE_ADDR2;
		// counter=3'b110;
	end
	
	if  (state==STATE_STOP&& ID !=2'b11)
	begin
		if (delay[2:0]==5'd0)
		begin
			SCL=0;
			delay<=delay+5'b001;
		end
		 else if (delay[2:0]==5'b00110)
		begin
			SDA=0;
			delay<=delay+5'b001;
		end
		else if (delay[2:0]==5'b01100) begin
			SCL=1;
			delay<=delay+5'b001;
		 end
		 else if (delay[4:0]==5'b10010) begin
			SDA=1; 
			delay<=delay+5'b001;
			ID =2'b11;
		 end
		 else begin
			delay<=delay+5'b001;
		end
	end	
end

always @(negedge clk_100k) begin // posedge à la base
#1
	if (stateMachineOn==1) begin
		case(state)
			STATE_IDLE:begin
				SDA=1; 
				SCL=1;
				counter=3'b110; ////a la base dans start
				state =STATE_START;	
			end

			
			STATE_ADDR:begin
				// SDA =address[counter];
				
				if (counter==3'b000) begin state<=STATE_RW; end
				else begin counter = counter - 1; end
			end
			
			STATE_RW:begin
				// #1 SDA=1'b0;// Always Write to select a register
				state=STATE_ACK_ADDR;
			end
			
			STATE_ACK_ADDR:begin
				//led_0_o<=SDA; //if byte not acknowledged the led will turn on
				counter=3'b111;
				state=STATE_REG;
			end
			
			STATE_REG:begin  // a modifier
				// #1 SDA = register[counter];
				if (counter==3'b000) 
				begin 
					#1 state=STATE_ACK_REG; 
				end 
				else 
				begin 
					#1 counter<=counter-1;
				end
			end
			
			STATE_ACK_REG:begin
				//led_1_o<=SDA; //if byte not acknowledged the led will turn on
				if (Read_Write==1) begin
				#2 SCL=0;
				counter=3'b110;
				#2 state=STATE_REPEAT_START;
				end 
				else begin 
					state<=STATE_DATA;
					counter<=3'b111;
				end
			end
			
			STATE_DATA:begin
				// SDA <= data[7];
				if (counter==3'b000) 
				begin
					state <= STATE_STOP; // pas normal
				end
				else begin
					counter<=counter-1;
				end
			end
			
			/*STATE_STOP:begin
				// Part<=2'b00;
				address<=7'd0;
				data<=8'd0;
				register<=8'd0;
				counter<=3'd0;

				// stateMachineOn=0; now in part process
				state=STATE_IDLE;
			end*/
			
			STATE_ADDR2: begin
				// SDA <=address[counter];
				if (counter==3'b000) begin state<=STATE_RW2; end
				else begin counter <= counter - 1; end
			end
			
			STATE_RW2:begin
				// SDA<=1'b0;// Always Write to select a register
				state<=STATE_ACK_ADDR2;
			end
			
			STATE_ACK_ADDR2:begin
				//led_0_o<=SDA; //if byte not acknowledged the led will turn on
				counter<=3'b111;
				DOUT_S<=8'd0;
				state<=STATE_READ;
			end
			
			STATE_READ:begin
				DOUT_S [counter]=sda_din;
				if(counter==3'b000) begin
					state<=STATE_MASTER_ACK;
				end
				else begin
					counter<=counter-1;
				end
			end
			
			STATE_MASTER_ACK:begin
			//SDA=1'b1;
			state<=STATE_STOP;
			end
		endcase
		
		case (ID) 
			1:begin	state<=STATE_ADDR; end
			2:begin state<=STATE_ACK_ADDR2; end
			3:begin state <= STATE_IDLE; end
		endcase
	end
	
end


endmodule