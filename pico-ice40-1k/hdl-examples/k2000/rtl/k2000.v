//
// eCow-logic - FPGA demo/example "K2000"
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
  input  clk_i,
  output led_0_o,
  output led_1_o,
  output led_2_o,
  output led_3_o,
  output led_4_o,
  output led_5_o,
  output led_6_o,
  output led_7_o
);

wire [7:0]leds;

// Divide main clock to produce led clock
reg [24:0] counter;

always @(posedge clk_i)
begin
    counter[24:0] <= counter[24:0] + 25'd1;
end

//
// State Machine
//

// Set states from divided clock
wire [3:0] steps;
assign steps = counter[24:21];
// Define patterns from states
assign leds[7:0] = (steps[3:0] == 4'b0000) ? 8'b00000001 :
                   (steps[3:0] == 4'b0001) ? 8'b00000010 :
                   (steps[3:0] == 4'b0010) ? 8'b00000100 :
                   (steps[3:0] == 4'b0011) ? 8'b00001000 :
                   (steps[3:0] == 4'b0100) ? 8'b00010000 :
                   (steps[3:0] == 4'b0101) ? 8'b00100000 :
                   (steps[3:0] == 4'b0110) ? 8'b01000000 :
                   (steps[3:0] == 4'b0111) ? 8'b10000000 :
                   (steps[3:0] == 4'b1000) ? 8'b01000000 :
                   (steps[3:0] == 4'b1001) ? 8'b00100000 :
                   (steps[3:0] == 4'b1010) ? 8'b00010000 :
                   (steps[3:0] == 4'b1011) ? 8'b00001000 :
                   (steps[3:0] == 4'b1100) ? 8'b00000100 :
                   (steps[3:0] == 4'b1101) ? 8'b00000010 :
                   (steps[3:0] == 4'b1110) ? 8'b00000001 : 8'h00;

// Connect all outputs leds from bus
assign led_7_o = leds[7];
assign led_6_o = leds[6];
assign led_5_o = leds[5];
assign led_4_o = leds[4];
assign led_3_o = leds[3];
assign led_2_o = leds[2];
assign led_1_o = leds[1];
assign led_0_o = leds[0];

endmodule
