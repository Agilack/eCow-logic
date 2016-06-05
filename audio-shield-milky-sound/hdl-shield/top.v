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
  
  //SPI Stuff
  output spi_miso,
  input  spi_mosi,
  input  spi_sck,
  input  spi_cs,
  
  output i2c_scl,
  inout  i2c_sda_io
  
  
);


localparam STATE_IDLE=0;
localparam STATE_I2C=1;

 wire i2c_recopy;
 assign i2c_recopy =RxReady;
 reg i2c_reset;
  
  wire [7:0]i2c_state;
  
wire [7:0] DTo_I2C;
wire [7:0] DFrom_I2C; // cc
reg [7:0] Dsaved_I2C;

reg clk_i2c=1'b0;

wire [7:0] DFrom_SPI;
reg [7:0] Dsaved_SPI;
reg [7:0] DTo_SPI;
wire RxReady;


reg [6:0] div_100k;
reg [25:0] div_1Hz;
reg state =STATE_IDLE;


initial
begin
	div_1Hz<=0;
	div_100k<=0;
	#2 i2c_reset=1;
	#2 i2c_reset=0;
end

always @(posedge clk)
begin
    DTo_SPI [7:0] <= DFrom_I2C[7:0]; 
	Dsaved_SPI [7:0] <= DFrom_SPI[7:0];
end

assign DTo_I2C [7:0]=DFrom_SPI[7:0];

assign led_7_o = i2c_state[7];
assign led_6_o = i2c_state[6];
assign led_5_o = i2c_state[5];
assign led_4_o = i2c_state[4];
assign led_3_o = i2c_state[3];
assign led_2_o = i2c_state[2];
assign led_1_o = i2c_state[1];
assign led_0_o = i2c_state[0];

always @(posedge clk)
begin
    div_100k <= div_100k + 1;
    if (div_100k == 7'b0011001) // normalement 250
    begin
        clk_i2c<= ~clk_i2c;
        div_100k <= 7'd0;
    end
    
end


always @(posedge clk)
begin
    div_1Hz <= div_1Hz + 1;
    if (div_1Hz == 26'b1011111010111100001000000 && state ==STATE_I2C)
    begin
        state <= STATE_IDLE;
    end 
end

always @(negedge RxReady)
begin
    if (state==STATE_IDLE)
    begin
        if (DFrom_SPI[7]==1 && DFrom_SPI[6]==1) // si les deux premiers bits en sortant du mode IDDLE sont à 1 alors mode I2C
        begin
            state <=STATE_I2C;
            i2c_reset<=1;
        end
    end
    if (state == STATE_I2C)
    begin
		// repeat (2) // BRRRRRRIIIIIIIIIIIIICOLAGE
		// begin
			// @(posedge clk);
		// end
		#1 i2c_reset = 0;
        // i2c_recopy = 1 ;
    end 
div_1Hz<=26'd0;
end


// always @(negedge RxReady) // GSG or posedge clk_i2c)
// begin
    // i2c_recopy<= 0 ;
// end

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

