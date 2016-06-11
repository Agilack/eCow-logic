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
`timescale 1ns / 1ps
module vga_sync(
        input        rst_i,
        input        clk_i,
        input        enable_i,
        output       valid_o, 
        output [9:0] pos_x_o, 
        output [9:0] pos_y_o, 
        output       sync_vs, 
        output       sync_hs  
);
  
parameter H_FP     =  56; // Front porch (in pixels-clocks)
parameter H_BP     =  64; // Back porch (in pixels-clocks) 
parameter H_PULSE  = 120; // HS pulse length (in pixels-clocks)
parameter H_PIXELS = 800; // Whole line
parameter V_FP     =  37; // Front porch (in lines)
parameter V_PULSE  =   6; // VS pulse length (in lines)
parameter V_BP     =  23; // Back porch (in lines)
parameter V_LINES  = 600; // Whole line

reg [10:0] hs_t_front;
reg [10:0] hs_t_pulse;
reg [10:0] hs_t_back; 
reg [10:0] hs_t_total;
reg [ 9:0] vs_t_front;
reg [ 9:0] vs_t_pulse;
reg [ 9:0] vs_t_back; 
reg [ 9:0] vs_t_total;
reg [10:0] count_hs = 11'h0;
reg [ 9:0] count_vs = 10'h0;

reg vid_hs          = 1'b1;
reg vid_vs          = 1'b1;
assign sync_vs = vid_vs;   
assign sync_hs = vid_hs;   

reg  vid_hvalid     = 1'b0;
reg  vid_vvalid     = 1'b0;
wire valid;
assign valid = (vid_hvalid && vid_vvalid);
assign valid_o = valid;

reg [9:0] pos_x;
reg [9:0] pos_y;
assign pos_x_o = pos_x;
assign pos_y_o = pos_y;

//
// Horizontal sync
//
wire hs_last;
assign hs_last = (count_hs == hs_t_total);

always @(posedge clk_i)
begin
        if (rst_i) begin
                hs_t_front <= H_FP;
                hs_t_pulse <= H_FP + H_PULSE;
                hs_t_back  <= H_FP + H_PULSE + H_BP;
                hs_t_total <= H_FP + H_PULSE + H_BP + H_PIXELS;
                vs_t_front <= V_FP;
                vs_t_pulse <= V_FP + V_PULSE;
                vs_t_back  <= V_FP + V_PULSE + V_BP;
                vs_t_total <= V_FP + V_PULSE + V_BP + V_LINES;
        end else if (enable_i) begin
                if (hs_last)
                        count_hs <= 11'd0;
                else
                        count_hs <= count_hs + 1'd1;

                /* Start HS pulse after the front porch */
                if (count_hs == hs_t_front)
                        vid_hs <= 1'b0;
                /* End of HS pulse (start of back porch) */
                if (count_hs == hs_t_pulse)
                        vid_hs <= 1'b1;
                /* End of back porch */
                if (count_hs == hs_t_back)
                        vid_hvalid <= 1'b1;
                /* End od visible pixels */
                if (hs_last)
                        vid_hvalid <= 1'b0;
        end
end

//
// Vertical sync
//
wire vs_last;
assign vs_last = (count_vs == vs_t_total);

always @(posedge clk_i)
begin
        if (enable_i) begin
                if (hs_last) begin  
                        if (vs_last)
                                count_vs <= 10'd0;
                        else
                                count_vs <= count_vs + 1'd1;
                end
                /* Start of VS pulse after the front porch */
                if (count_vs == vs_t_front)
                        vid_vs <= 1'b0;
                /* End of VS pulse (start of back porch) */
                if (count_vs == vs_t_pulse)
                        vid_vs <= 1'b1;
                /* Start of visible pixels , end of back porch */
                if (count_vs == vs_t_back) 
                        vid_vvalid <= 1'b1;
                /* End of visible pixels */
                if (vs_last)
                        vid_vvalid <= 1'b0;
        end // if enable_i
end

//
// Create X and Y coord from sync signals
//
always @(posedge clk_i)
begin
        if (count_vs == 0)
                pos_y <= 10'h0;
        else if (vid_vvalid && (count_hs == 0))
                pos_y <= pos_y + 10'd1;
        
        if (count_hs == 0)
                pos_x <= 10'b0;
        else if (valid)
                pos_x <= pos_x + 10'd1;
end
   
endmodule
