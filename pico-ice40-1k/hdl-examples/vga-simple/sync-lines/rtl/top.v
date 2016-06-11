//
// eCow-logic - FPGA demo/example - VGA sync and color lines
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
  // Main oscillator (25MHz)
  input  clk_i,
  // LEDs
  output led_0_o,
  output led_1_o,
  output led_2_o,
  output led_3_o,
  output led_4_o,
  output led_5_o,
  output led_6_o,
  output led_7_o,
  // VGA output
  output vga_r0_o,
  output vga_r1_o,
  output vga_g0_o,
  output vga_g1_o,
  output vga_b0_o,
  output vga_b1_o,
  output vga_hs_o,
  output vga_vs_o
);


// Control signals
wire clk_vga;
wire rst_sys;
// VGA internal signals
wire [9:0] pos_x;
wire [9:0] pos_y;
wire vga_disp_en;
wire [5:0] array_color;

assign vga_r0_o = vga_disp_en ? array_color[0] : 1'b0;
assign vga_r1_o = vga_disp_en ? array_color[3] : 1'b0;
assign vga_g0_o = vga_disp_en ? array_color[1] : 1'b0;
assign vga_g1_o = vga_disp_en ? array_color[4] : 1'b0;
assign vga_b0_o = vga_disp_en ? array_color[2] : 1'b0;
assign vga_b1_o = vga_disp_en ? array_color[5] : 1'b0;

assign array_color[2:0] = pos_x[8:6];
assign array_color[5:3] = pos_y[8:6];

// ----------------------------------------------------------------------------
// --                                 Reset                                  --
// ----------------------------------------------------------------------------
reg [3:0] rst_wait = 4'b1111;
reg rst;
always @(posedge clk_i)
begin
    if (rst_wait == 4'b0000)  
        rst <= 1'b0;
        
    else begin
        rst <= 1'b1;
        rst_wait <= rst_wait - 4'd1;
    end
end
assign rst_sys = rst;

// ----------------------------------------------------------------------------
// --                       Generate VGA sync signals                        --
// ----------------------------------------------------------------------------
vga_sync #(
        .H_FP     (  16 ), // Front porch (in pixels-clocks)
        .H_BP     (  48 ), // Back porch (in pixels-clocks)
        .H_PULSE  (  96 ), // HS pulse length (in pixels-clocks)
        .H_PIXELS ( 640 ), // Whole line
        .V_FP     (  10 ), // Vertical front porch (in lines)
        .V_BP     (  33 ), // Vertical back porch (in lines)
        .V_PULSE  (   2 ), // VS pulse length
        .V_LINES  ( 480 )  // Number of lines
    ) sync_u (
        .rst_i   ( rst_sys     ),
        .clk_i   ( clk_i       ),
        //
        .enable_i( 1'b1        ),
        .valid_o ( vga_disp_en ),
        //
        .pos_x_o ( pos_x       ),
        .pos_y_o ( pos_y       ),
        //
        .sync_vs ( vga_vs_o    ),
        .sync_hs ( vga_hs_o    )
);

endmodule
