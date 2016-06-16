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
  input Bclk
);


localparam STATE_IDLE=1'b0;
localparam STATE_I2C=1'b1;

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

//SPI signals
wire [7:0] DFrom_SPI;
reg [7:0] Dsaved_SPI;
reg [7:0] DTo_SPI;
wire RxReady;

reg [7:0] div_100k=8'd0;
reg [25:0] div_1Hz;
reg state =STATE_IDLE;

//I2S data register
reg [31:0] I2SData=32'hAACCF502;
wire [7:0] i2s_debug;


always @(posedge clk)
begin
    DTo_SPI [7:0] <= DFrom_I2C[7:0]; 
	Dsaved_SPI [7:0] <= DFrom_SPI[7:0];
end

assign DTo_I2C [7:0]=DFrom_SPI[7:0];

// debug 
assign led_7_o = Wclk;
assign led_6_o = Bclk;
assign led_5_o = i2s_debug[7];
assign led_4_o = i2s_debug[6];
assign led_3_o = i2s_debug[5];
assign led_2_o = i2s_debug[4];
assign led_1_o = i2s_debug[3];
assign led_0_o = i2s_debug[2];

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
reg pause;

always @(posedge clk)// or posedge RxReady)
begin
	if (RxReady==1) 
	begin
		div_1Hz<=26'd0;
		if (state==STATE_IDLE&&(DFrom_SPI[7]==1 && DFrom_SPI[6]==1)) // si les deux premiers bits en sortant du mode IDDLE sont à 1 alors mode I2C
		begin 
			pause <=STATE_I2C;
		end
	end 
	else if (pause==STATE_I2C) 
	begin 
		state <= STATE_I2C;
		pause <= STATE_IDLE;
	end
	else if (div_1Hz == 26'b1011111010111100001000000 && state ==STATE_I2C)
		begin
			state <= STATE_IDLE;
			pause<=STATE_IDLE;
		end
	else
		begin
			div_1Hz <= div_1Hz + 1;
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
    


endmodule

