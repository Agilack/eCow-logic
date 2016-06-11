//
// eCow-logic - FPGA demo/example - Drive LEDs with SPI datas
//
// Copyright (c) 2016 Saint-Genest Gwenael <gwen@agilack.fr>
//
// This file may be distributed and/or modified under the terms of the
// GNU General Public License version 2 as published by the Free Software
// Foundation. (See COPYING.GPL for details.)
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
module top(
  // LEDs
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
  // SPI slave port
  output spi_miso_o,
  input  spi_mosi_i,
  input  spi_sck_i,
  input  spi_cs_i
);

// Counter of SPI clock ticks
reg [2:0] spi_bits = 3'b000;
// This is the value sent over SPI
reg [7:0] spi_tx;
// This is the buffer for received bits
reg [7:0] spi_rx;

// Registered value of push buttons
reg [4:1] buttons;

always @(posedge spi_sck_i)
begin
        buttons[4:1] <= { but_4_i, but_3_i, but_2_i, but_1_i };
end

assign spi_tx[7:0] = { 4'hA, buttons[4:1] };

////////////////////////////////////////////////////////////////////////////////
//                                     TX                                     //
////////////////////////////////////////////////////////////////////////////////
always @(negedge spi_sck_i or posedge spi_cs_i)
begin
    if (spi_cs_i) begin
        spi_bits[2:0] <= 3'b0;
    end else begin
        spi_bits[2:0] <= spi_bits[2:0] + 3'd1;
    end
end

assign spi_miso_o = (spi_bits[2:0] == 3'b000) ? spi_tx[7] :
                    (spi_bits[2:0] == 3'b001) ? spi_tx[6] :
                    (spi_bits[2:0] == 3'b010) ? spi_tx[5] :
                    (spi_bits[2:0] == 3'b011) ? spi_tx[4] :
                    (spi_bits[2:0] == 3'b100) ? spi_tx[3] :
                    (spi_bits[2:0] == 3'b101) ? spi_tx[2] :
                    (spi_bits[2:0] == 3'b110) ? spi_tx[1] :
                    spi_tx[0];

////////////////////////////////////////////////////////////////////////////////
//                                     RX                                     //
////////////////////////////////////////////////////////////////////////////////
always @(posedge spi_sck_i)
begin
        spi_rx[7:0] <= {spi_rx[6:0], spi_mosi_i};
end

assign led_0_o = spi_rx[0];
assign led_1_o = spi_rx[1];
assign led_2_o = spi_rx[2];
assign led_3_o = spi_rx[3];
assign led_4_o = spi_rx[4];
assign led_5_o = spi_rx[5];
assign led_6_o = spi_rx[6];
assign led_7_o = spi_rx[7];

endmodule
