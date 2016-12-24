`timescale 1ns / 100ps
module sram(
    input  clk_i,
    // Wires to SRAM chip
    output oe_o,
    output ce_o,
    output we_o,
    output [18:0] ad_o,
    inout  [ 7:0] dq_io,
    // Wishbone interface
    input         wb_cyc_i,
    input         wb_we_i,
    output        wb_ack_o,
    input  [18:0] wb_adr_i,
    input  [ 7:0] wb_dat_i,
    output [ 7:0] wb_dat_o
);

reg  oe = 1'b1;
reg  ce = 1'b1;
reg  we = 1'b1;
reg  dq_dir = 1'b0;
reg  [18:0] ad_r = 19'b0;
reg  [ 7:0] d_o;
wire [ 7:0] dq_in;
wire [ 7:0] dq_out;
reg  wb_ack = 1'b0;
reg  [ 2:0] stm = 3'b0;
reg  [ 7:0] d_wr;
assign dq_out = d_wr;

always @(posedge clk_i)
begin
    case (stm)
        // State IDLE
        3'b0_00: begin
            wb_ack <= 1'b0;
            if (wb_cyc_i == 1'b1) begin
                ad_r[18:0] <= wb_adr_i[18:0];
                if (wb_we_i == 1'b0) begin
                    ce <= 1'b0;
                    oe <= 1'b0;
                    stm[2:0] <= 3'b0_01;
                end else begin
                    ce <= 1'b0;
                    we <= 1'b0;
                    stm[2:0] <= 3'b1_00;
                end
            end
        end
        // READ dummy cycle (wait datas)
        3'b0_01: begin
            stm[2:0] <= 3'b0_10;
        end
        // READ get datas
        3'b0_10: begin
            d_o[7:0] <= dq_in[7:0];
            oe <= 1'b1;
            ce <= 1'b1;
            wb_ack <= 1'b1;
            if (wb_cyc_i == 1'b0)
                stm[2:0] <= 3'b0_00;
        end
        //
        3'b0_11: begin
        end
        // WRITE - Wait tWHZ
        3'b1_00: begin
            // Set Data port to output
            dq_dir   <= 1'b1;
            // Set byte to write
            d_wr     <= wb_dat_i;
            
            stm[2:0] <= 3'b1_01;
        end
        // WRITE - dummy cycle
        3'b1_01: begin
            stm[2:0] <= 3'b1_10;
        end
        // WRITE - complete
        3'b1_10: begin
            dq_dir <= 1'b0;
            ce     <= 1'b1;
            we     <= 1'b1;
            // End of wishbone transfer
            wb_ack <= 1'b1;
            if (wb_cyc_i == 1'b0)
                stm[2:0] <= 3'b0_00;
        end
        // Unused state
        3'b1_11: begin end
    endcase
end

assign oe_o = oe;
assign ce_o = ce;
assign we_o = we;
assign wb_ack_o = wb_ack;
assign ad_o[18:0] = ad_r[18:0];
assign wb_dat_o[7:0] = d_o[7:0];

// ---------- IO Buffers ----------

SB_IO #(
    .PIN_TYPE(6'b1010_01),
    .PULLUP  ( 1'b0     )
) iob_dq0 (
    .PACKAGE_PIN  (dq_io[0]),
    .OUTPUT_ENABLE(dq_dir),
    .D_OUT_0      (dq_out[0]),
    .D_IN_0       (dq_in [0])
);

SB_IO #(
    .PIN_TYPE(6'b1010_01),
    .PULLUP  ( 1'b0     )
) iob_dq1 (
    .PACKAGE_PIN  (dq_io[1]),
    .OUTPUT_ENABLE(dq_dir),
    .D_OUT_0      (dq_out[1]),
    .D_IN_0       (dq_in [1])
);

SB_IO #(
    .PIN_TYPE(6'b1010_01),
    .PULLUP  ( 1'b0     )
) iob_dq2 (
    .PACKAGE_PIN  (dq_io[2]),
    .OUTPUT_ENABLE(dq_dir),
    .D_OUT_0      (dq_out[2]),
    .D_IN_0       (dq_in [2])
);

SB_IO #(
    .PIN_TYPE(6'b1010_01),
    .PULLUP  ( 1'b0     )
) iob_dq3 (
    .PACKAGE_PIN  (dq_io[3]),
    .OUTPUT_ENABLE(dq_dir),
    .D_OUT_0      (dq_out[3]),
    .D_IN_0       (dq_in [3])
);

SB_IO #(
    .PIN_TYPE(6'b1010_01),
    .PULLUP  ( 1'b0     )
) iob_dq4 (
    .PACKAGE_PIN  (dq_io[4]),
    .OUTPUT_ENABLE(dq_dir),
    .D_OUT_0      (dq_out[4]),
    .D_IN_0       (dq_in [4])
);

SB_IO #(
    .PIN_TYPE(6'b1010_01),
    .PULLUP  ( 1'b0     )
) iob_dq5 (
    .PACKAGE_PIN  (dq_io[5]),
    .OUTPUT_ENABLE(dq_dir),
    .D_OUT_0      (dq_out[5]),
    .D_IN_0       (dq_in [5])
);

SB_IO #( .PIN_TYPE(6'b1010_01), .PULLUP( 1'b0 ) ) 
    iob_dq6 (.PACKAGE_PIN(dq_io[6]), .OUTPUT_ENABLE(dq_dir), .D_OUT_0(dq_out[6]), .D_IN_0(dq_in[6]) );

SB_IO #( .PIN_TYPE(6'b1010_01), .PULLUP( 1'b0 ) )
    iob_dq7 (.PACKAGE_PIN(dq_io[7]), .OUTPUT_ENABLE(dq_dir), .D_OUT_0(dq_out[7]), .D_IN_0(dq_in[7]) );

endmodule
