`timescale 10ns/1ns
//
//
// Copyright (c) 2016 Briand Cyril <cyril.briand@agilack.fr>
//
// This file may be distributed and/or modified under the terms of the
// GNU General Public License version 2 as published by the Free Software
// Foundation. (See COPYING.GPL for details.)
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//

module top(

  input clk,

  output led_0_o,
  output led_1_o,
  output led_2_o,
  output led_3_o,
  output led_4_o,
  output led_5_o,
  output led_6_o,
  output led_7_o,
  // Push Buttons
  input  but_1_i,
  input  but_2_i,
  input  but_3_i,
  input  but_4_i,
  
  //SPI IN Stuff
  output spi_miso,
  input  spi_mosi,
  input  spi_sck,
  input  spi_cs,
  
  output spi_select,
  output micbias,
  
  //I2C signals
  output i2c_scl,
  inout  i2c_sda_io,
  
  output SD_ampli,
  output reset_DAC,
  //I2S signals
  output DToDAC,
  output Mclk,
  input Wclk,
  input Bclk,
  // ram signals
  output ram_oe_o,
  output ram_ce_o,
  output ram_we_o,
  inout  [7:0]  ram_dq_io,
  output [18:0] ram_a_o
);


localparam STATE_IDLE=2'b00;
localparam STATE_I2C=2'd1;
localparam STATE_RAM=2'd2;
localparam STATE_I2S=2'd3;

//  untouch  signals to shield
assign spi_select = 1'b0;
assign micbias =1'bz;
assign Mclk = clk; 
assign reset_DAC = 1'b1;
assign SD_ampli =1'b0;

//I2C signals
 wire i2c_recopy;
 assign i2c_recopy =(state==STATE_I2C) ? RxReady : 0;
 wire i2c_reset;
 assign i2c_reset = (state==STATE_I2C) ? 0 :1;
  
  wire [7:0]i2c_state;
  
wire [7:0] DTo_I2C;
wire [7:0] DFrom_I2C; // cc
reg [7:0] Dsaved_I2C;

reg clk_i2c=1'b0;

//RAM Signals
wire [18:0] wb_adr;
reg wb_cyc;
wire wb_we;
wire wb_ack;

reg [18:0] address_ram;
wire [18:0] wb_dFromRam;
reg [3:0] state_ram=4'b0000;
reg [7:0] wb_dToRam=8'd0;
reg [18:0] final_address;
//SPI signals
wire [7:0] DFrom_SPI;

reg [7:0] DTo_SPI;
wire RxReady;
wire [2:0]spi_count_o;

reg [7:0] div_100k=8'd0;
reg [25:0] div_1Hz;
reg [1:0] state =STATE_IDLE;

//I2S data register
reg [31:0] I2SData=32'hAACCF502;
wire [7:0] i2s_debug;

always @(posedge clk)
begin
	if(state==STATE_I2C)
		DTo_SPI [7:0] <= DFrom_I2C[7:0]; 
end

assign DTo_I2C [7:0]=(state==STATE_I2C ) ? DFrom_SPI[7:0] : 8'h00;

// debug 
assign led_7_o = state[1];
assign led_6_o = state[0];
assign led_5_o = (address_ram==19'd44) ? 1'b1:1'b0;
assign led_4_o = i2s_debug[5];
assign led_3_o = state_ram[3];
assign led_2_o = state_ram[2];
assign led_1_o = state_ram[1];
assign led_0_o = state_ram[0];

//WITH REEL CLOCK 
always @(posedge clk)
begin
    div_100k <= div_100k + 8'd1;
    if (div_100k == 8'b11111010) 
    begin
        clk_i2c<= ~clk_i2c;
        div_100k <= 8'd0;
    end
    
end

//prevent the first Byte to go to I2C
reg [1:0] pause;

always @(posedge clk)// or posedge RxReady)
begin
	if (RxReady==1) 
	begin
	div_1Hz<=26'd0;
	    if (state==STATE_I2S)
	    begin
	        state<=STATE_IDLE;
        end
        else 
		if (state==STATE_IDLE&&(DFrom_SPI[7]==1 && DFrom_SPI[6]==1)) // si les deux premiers bits en sortant du mode IDDLE sont à 1 alors mode I2C
		begin 
			pause <=STATE_I2C;
		end
		else if(state==STATE_IDLE&&(DFrom_SPI[7]==0 && DFrom_SPI[6]==1))
		begin
			pause <= STATE_RAM;
		end
		else if (state==STATE_IDLE&&(DFrom_SPI[7]==0 && DFrom_SPI[6]==0))
		begin
		    pause <= STATE_I2S;
		end
	end 
	else begin
    	if (pause==STATE_I2S)
    	begin
    			state<=STATE_I2S;
    			pause <= STATE_IDLE;
    	end
    	else if (pause==STATE_I2C) 
    	begin 
    		state <= STATE_I2C;
    		pause <= STATE_IDLE;
    	end
    	else if (pause == STATE_RAM)
    	begin
    		state <= STATE_RAM;
    		pause <= STATE_IDLE;
    	end
    	  else if (div_1Hz == 26'b1011111010111100001000000 && (state ==STATE_I2C ||state==STATE_RAM))// reel 
    //	else if (div_1Hz == 26'd100000 && (state ==STATE_I2C ||state==STATE_RAM)) //simu
    	begin
    	   if (state==STATE_RAM)
    	   begin
    	       final_address[18:0]<=address_ram[18:0];
    	    end
    		state <= STATE_IDLE;
    		pause<=STATE_IDLE;
    	end
    	else
    	begin
    		div_1Hz <= div_1Hz + 1;
    	end
	end
end

assign wb_we =(state==STATE_RAM) ? 1'b1 : 1'b0;
assign wb_adr [18:0] = address_ram [18:0];

// state machine to save or read the datas in the SRAM
always @(posedge clk)
begin
	if ((address_ram[18:0]<final_address[18:0] && state==STATE_I2S)||state==STATE_RAM)	   
	begin
		case (state_ram)
			4'b0000:begin
				if (state==STATE_I2S) // Read SRAM sequence
				begin
					if (Wclk==1'b1)
						state_ram<=4'b0001;
				end
				else if (state==STATE_RAM) // Write SRAM sequence
				begin
					if (RxReady==1'b1)
						state_ram<=4'b1010;
				end
			end	
			4'b0001:begin
				if(wb_ack==1'b0)
					begin
					state_ram<=4'b0010;
					wb_cyc<=1'b1; 
				end
			end
			4'b0010:begin
				I2SData[23:16] <= wb_dFromRam[7:0];
				if(wb_ack==1'b1)
				begin
					state_ram <=4'b0011;
					wb_cyc<=1'b0;
					address_ram<=address_ram+19'd1;
				end
				else if (wb_cyc==1'b0)
				begin
					state_ram<=4'b0011;
					address_ram<=address_ram+19'd1;
				end
			end
			4'b0011:begin
				if(wb_ack==1'b0) 
					begin
					state_ram<=4'b0100;
					wb_cyc<=1'b1; 
				end
			end
			4'b0100:begin
				I2SData[31:24] <= wb_dFromRam[7:0];


				if(wb_ack==1'b1)
				begin
					state_ram <=4'b0101;
					wb_cyc<=1'b0;
					address_ram<=address_ram+19'd1;
				end
				else if (wb_cyc==1'b0)
				begin
					state_ram<=4'b0101;
					address_ram<=address_ram+19'd1;
				end
			end
			4'b0101:begin
				if (Wclk==1'b0)
					state_ram<=4'b0110;
			end
			4'b0110:begin
				if(wb_ack==1'b0) //New
					begin


					state_ram<=4'b0111;
					wb_cyc<=1'b1; 
				end
			end
			4'b0111:begin

				I2SData[7:0] <= wb_dFromRam[7:0];

				if(wb_ack==1'b1)
				begin
					state_ram <=4'b1000;
					wb_cyc<=1'b0;
					address_ram<=address_ram+19'd1;
				end
				else if (wb_cyc==1'b0)
				begin
					state_ram<=4'b1000;
					address_ram<=address_ram+19'd1;
				end
			end
			4'b1000:begin
				if(wb_ack==1'b0) //New
					begin


					state_ram<=4'b1001;
					wb_cyc<=1'b1; 
				end
			end
			4'b1001:begin

				I2SData[15:8] <= wb_dFromRam[7:0];

				if(wb_ack==1'b1)
				begin
					state_ram <=4'b0000;
					wb_cyc<=1'b0;
					address_ram<=address_ram+19'd1;
				end
				else if (wb_cyc==1'b0)
				begin
					state_ram<=4'b0000;
					address_ram<=address_ram+19'd1;
				end
			end
		4'b1010:begin
			if (RxReady==1'b0)
				begin
					wb_dToRam[7:0] <= DFrom_SPI[7:0];
					state_ram<=4'b1011;
				end
			end
		4'b1011:begin
			wb_cyc<=1'b1;
			if(wb_ack==1'b1)
				begin
					state_ram <=4'b1100;
				end
			end
		4'b1100:begin
				wb_cyc<=1'b0;
				address_ram <=address_ram +19'd1;
				state_ram<=4'b0000;
			end	
		endcase
	end
	else if (state != STATE_RAM)

	begin
	 if(state==STATE_I2S && final_address>19'd44)
	   begin
    	address_ram[18:0]<=19'd44;
        state_ram<=4'b0000;
	   end
	   else
	   begin
        	address_ram[18:0]<=19'd0;
        	state_ram<=4'b0000;
        end
	end
end


i2s i2s_a
	(
		.clk(clk),
		.DLeft(I2SData[31:16]),
		.DRight(I2SData[15:0]),
		.Wclk(Wclk),
		.Bclk(Bclk),
		.debug(i2s_debug),
		.i2s_out(DToDAC)
	);
	
spi spi_a
    (
        .clk_i(clk),
        .newOctet ( RxReady ),
        .writeGlu ( DFrom_SPI ),
        .readGlu  ( DTo_SPI  ),
        .spi_miso_o( spi_miso),
        .spi_mosi_i (spi_mosi),
		.spi_bits_o(spi_count_o),
        .spi_sck_i (spi_sck),
        .spi_cs_i(spi_cs)
        
    );

i2c i2c_a
    (
		//
		.clk(clk),
		.i2c_state(i2c_state),
        .clk_100k (clk_i2c),
        .DIN(DTo_I2C),
        .DOUT(DFrom_I2C),
        .copy_enable(i2c_recopy),
        .i2c_scl(i2c_scl),
        .i2c_sda_io(i2c_sda_io),
        .reset(i2c_reset)
    );
    
sram sram_u (
    .clk_i   ( clk   ),
    // Chip signals
    .oe_o    ( ram_oe_o  ),
    .ce_o    ( ram_ce_o  ),
    .we_o    ( ram_we_o  ),
    .ad_o    ( ram_a_o [18:0] ),
    .dq_io   ( ram_dq_io[7:0] ),
    // Wishbone signals
    .wb_cyc_i( wb_cyc       ),
    .wb_we_i ( wb_we        ),
    .wb_ack_o( wb_ack       ),
    .wb_adr_i( wb_adr[18:0] ),
    .wb_dat_i( wb_dToRam [7:0]),
    .wb_dat_o( wb_dFromRam[7:0] )

);

endmodule

