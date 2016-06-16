`timescale 10ns/1ns
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
  inout i2c_sda_io,
  output i2c_scl,
  input [7:0] DIN,
  output [7:0] DOUT,
  input reset,
  input copy_enable,
  output busy,
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


localparam STATE_IDLE=8'd0;
localparam STATE_START=8'd1;
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



reg SDA;
reg SCL;
reg [7:0] DIN_S;
reg [7:0] DOUT_S=8'h55;

reg [7:0] delay =8'b00000000;
wire done;

always @(clk_100k)
begin
	DIN_S [7:0]<=DIN [7:0];
end
assign 	DOUT[7:0]=DOUT_S [7:0];

reg [6:0] address;
reg [7:0] register;
reg [7:0] data;
reg [1:0] Part=2'b0;
reg [7:0] state;//=STATE_IDLE;
reg [2:0] counter; 

//
assign i2c_state[7:0] = {address[4:0],sda_oe,status,reset}; ///Debug normally state
assign done =(state==STATE_STOP) ? 1 : 0;

reg Read_Write;
reg stateMachineOn;
wire sda_status;




// Pilot the output enable of the pin SDA
assign sda_oe =(((state==STATE_IDLE)||(state==STATE_START)||(state==STATE_STOP)||(state==STATE_REPEAT_START)||(state==STATE_ADDR)||(state==STATE_ADDR2)||(state==STATE_REG)||(state==STATE_DATA)||(state==STATE_RW)||(state==STATE_RW2)||(state==STATE_MASTER_ACK))&& sda_status==1'b0) ? 1'b1 : 1'b0;


//Put i2c_sda in high impedance when read is needed
assign sda_dout=((state==STATE_IDLE)||(state==STATE_START)||(state==STATE_STOP)||(state==STATE_REPEAT_START)) ? SDA :
				((state==STATE_ADDR)||(state==STATE_ADDR2)) ? address[counter] : 
				(state==STATE_REG) ? register[counter]:
				(state==STATE_DATA) ? data[counter]:
				(state ==STATE_RW) ? 1'b0 :
				(state==STATE_RW2) ? Read_Write:
				(state==STATE_MASTER_ACK) ? 1'b1: 
				1'b1; //SDA
				

assign sda_status=sda_dout;
assign i2c_scl = SCL;

always @(negedge copy_enable or posedge done or posedge reset)
begin
	if  (done==1)
	begin
		stateMachineOn<=1'b0;
		Part<=2'b00;
		address<=7'd0;
		data<=8'd0;
		register<=8'd0;
	end else if (reset ==1)
	begin
		stateMachineOn<=1'b0;
		Part<=2'b00;
		address<=7'd0; 
		data<=8'd0; 
		register<=8'd0; 
	end else
	begin
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
end





always @(posedge clk) begin 
	if  (reset==1)
	begin
		state[7:0]<=STATE_IDLE;
		SDA<=1;
		SCL<=1;
		counter<=3'd0;
		delay<=8'b00000000;
		
	end
	else if (state== STATE_STOP)
	begin
		if (delay==8'b00000000)
		begin
			SCL=0;
			delay<=delay + 8'd1;
		end
		 else if (delay==8'd62)
		begin
			SDA=0;
			delay<=delay+8'd1;
		end
		else if (delay==8'd125) begin
			SCL=1;
			delay<=delay + 8'd1;
		 end
		 else if (delay==8'd187) begin
			SDA=1; 
			delay<=8'd0;
			counter<=3'd0;
			state=STATE_IDLE;
		 end
		 else begin
			delay<=delay + 8'd1;
		end
	end
	else if (stateMachineOn==1)
	begin
		case(state)
			STATE_IDLE:begin
				SDA<=1; 
				SCL<=1;
				state <=STATE_START;
			end
			
			STATE_START:begin
				if (delay==8'd187)
				begin
					counter<=3'b110;
					state<=STATE_ADDR;
					delay<=8'd0;
				end else begin
					if (delay[7:0]==8'b00000000)
					begin
						SDA<=0;
						delay<= delay + 8'b00000001;
					end
					else if (delay[7:0]==8'b01111101) // if delay=125
					begin
						SCL<=0;
						delay<= delay + 8'b00000001;
					end 
					delay<=delay + 8'b00000001;
				end
			end
			
			STATE_ADDR:begin
				if (counter==3'b000 && delay==8'd249) begin // danger
					state<=STATE_RW;
					delay<=8'd0;
				end
				else begin 
					if (delay==8'd62)
					begin
						SCL<=1;
						delay<= delay + 8'b00000001;
					end
					else if (delay==8'd187)
					begin
						SCL<=0;
						delay<= delay + 8'b00000001;
					end
					
					if (delay==8'd250)
					begin 
						counter = counter - 1; 
						delay<=8'b00000000;
					end
					else begin	delay<= delay + 8'b00000001;
					end
				end
			end
			
			STATE_RW:begin
				if (delay==8'd62)
				begin
					SCL<=1;
					delay<= delay + 8'b00000001;
				end
				else if (delay==8'd187)
				begin
					SCL<=0;
					delay<= delay + 8'b00000001;
				end
				if (delay==8'd250) begin
					state=STATE_ACK_ADDR;
					delay<=8'b00000000;
				end
				else begin
					delay<=delay + 8'b00000001;
				end
			end
			
			STATE_ACK_ADDR:begin
				if (delay==8'd62)
				begin
					SCL<=1;
					delay<= delay + 8'b00000001;
				end
				else if (delay==8'd187)
				begin
					SCL<=0;
					delay<= delay + 8'b00000001;
				end
				if (delay==8'd250) begin
					counter=3'b111;
					state=STATE_REG;
					delay<=8'b00000000;
				end
				else begin
					delay <=delay+ 8'b00000001;
				end
			end
			
			STATE_REG:begin  // a modifier
				if (counter==3'b000 && delay==8'd249) begin // danger
					state<=STATE_ACK_REG;
					delay<=8'd0;
				end
				else begin	
					if (delay==8'd62)
					begin
						SCL<=1;
						delay<= delay + 8'b00000001;
					end
					else if (delay==8'd187)
					begin
						SCL<=0;
						delay<= delay + 8'b00000001;
					end
					if (delay==8'd250)
					begin 
						counter = counter - 1; 
						delay<=8'b00000000;
					end
					else begin
						delay<= delay + 8'b00000001;
					end
				end
			end
			
			STATE_ACK_REG:begin
				if (delay==8'd62)
				begin
					SCL<=1;
					delay<= delay + 8'b00000001;
				end
				else if (delay==8'd187)
				begin
					SCL<=0;
					delay<= delay + 8'b00000001;
				end
				if (delay==8'd250) begin
					if (Read_Write==1) begin
						SCL<=0;
						state<=STATE_REPEAT_START;
						delay<=8'b00000000;
					end 
					else begin 
						state<=STATE_DATA;
						counter<=3'b111;
						delay<=8'b00000000;
					end
				end
				else begin
					delay <=delay+ 8'b00000001;
				end
			end
			
			STATE_DATA:begin
				if (counter==3'b000 && delay==8'd249) begin // danger
					state<=STATE_ACK_DATA;
					delay<=8'd0;
				end
				else begin	
					if (delay==8'd62)
					begin
						SCL<=1;
						delay<= delay + 8'b00000001;
					end
					else if (delay==8'd187)
					begin
						SCL<=0;
						delay<= delay + 8'b00000001;
					end
					if (delay==8'd250)
					begin 
						counter = counter - 1; 
						delay<=8'b00000000;
					end
					else begin	
						delay<= delay + 8'b00000001;
					end
				end
			end
			
			STATE_ACK_DATA:begin
				if (delay==8'd62)
				begin
					SCL<=1;
					delay<= delay + 8'b00000001;
				end
				else if (delay==8'd187)
				begin
					SCL<=0;
					delay<= delay + 8'b00000001;
				end
				if (delay==8'd250) begin
					state=STATE_STOP;
					delay<=8'b00000000;
				end
				else begin
					delay <=delay+ 8'b00000001;
				end
			end
			
			
			STATE_ADDR2: begin
				if (counter==3'b000 && delay==8'd249) begin // danger
					state<=STATE_RW2;
					delay<=8'd0;
				end
				else begin	
					if (delay==8'd62)
					begin
						SCL<=1;
						delay<= delay + 8'b00000001;
					end
					else if (delay==8'd187)
					begin
						SCL<=0;
						delay<= delay + 8'b00000001;
					end
					if (delay==8'd250)
					begin 
						counter = counter - 1; 
						delay<=8'b00000000;
					end
					else begin
						delay<= delay + 8'b00000001;
					end
				end
				
			end
			
			STATE_RW2:begin
				if (delay==8'd62)
				begin
					SCL<=1;
					delay<= delay + 8'b00000001;
				end
				else if (delay==8'd187)
				begin
					SCL<=0;
					delay<= delay + 8'b00000001;
				end
				else if (delay==8'd250) begin
					state=STATE_ACK_ADDR2;
					delay<=8'b00000000;
				end
				else begin 
					delay<=delay + 8'b00000001;
				end
			end
			
			STATE_ACK_ADDR2:begin
				if (delay==8'd62)
				begin
					SCL<=1;
					delay<= delay + 8'b00000001;
				end
				else if (delay==8'd187)
				begin
					SCL<=0;
					delay<= delay + 8'b00000001;
				end
				if (delay==8'd250) begin
					counter=3'b111;
					state=STATE_READ;
					delay<=8'b00000000;
				end
				else begin
					delay <=delay+ 8'b00000001;
				end
			end
			
			STATE_READ:begin
				if (counter==3'b000 && delay==8'd249) begin 
					state<=STATE_MASTER_ACK;
					delay<=8'd0;
				end
				else begin	
					if (delay==8'd62)
					begin
						SCL<=1;
						delay<= delay + 8'b00000001;
					end
					else if (delay==8'd125)
					begin
						DOUT_S [counter]=sda_din;
					end
					else if (delay==8'd187)
					begin
						SCL<=0;
						delay<= delay + 8'b00000001;
					end
					if (delay==8'd250)
					begin 
						counter = counter - 1; 
						delay<=8'b00000000;
					end
					else begin
						delay<= delay + 8'b00000001;
					end
				end
			end
			
			STATE_MASTER_ACK:begin
				if (delay==8'd62)
				begin
					SCL<=1;
				end
				else if (delay==8'd187)
				begin
					SCL<=0;
				end
				if (delay==8'd250) begin
					state=STATE_STOP;
					delay<=8'b00000000;
				end
				else begin
					delay <=delay+ 8'b00000001;
				end
			end
			
			STATE_REPEAT_START:begin
				if (delay==8'b00000000)
				begin
					SCL<=0;
					delay<=delay + 8'b00000001;
				end
				 else if (delay==8'd62)
				begin
					SDA<=1;
					delay<=delay + 8'b00000001;
				end
				else if (delay==8'd125) begin
					SCL<=1;
					delay<=delay + 8'b00000001;
				 end
				 else if (delay==8'd187) begin
					SDA<=0; 
					delay<=delay + 8'b00000001;
				 end
				 else if (delay==8'd250) begin
					SCL<=0;
					delay<=8'b00000000;
					state<=STATE_ADDR2;
					counter <= 3'b110;
				 end
				 else begin
					delay<=delay + 8'b00000001;
				end
			end
		endcase
	end
end


endmodule