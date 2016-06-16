`timescale 10ns/1ns

module i2s(
input clk,
input [15:0] DLeft,
input [15:0] DRight, 
input Wclk,
input Bclk,
output i2s_out,
output [7:0] debug
);


assign debug ={Wclk,Bclk,counter [4:2],i2s_out,1'b1,1'b1};

reg [4:0] counter=5'b10000;
reg ok;
reg last_ok;

always @(posedge Wclk or negedge Wclk)
begin
	if (Wclk == 1'b1) begin
	ok <= 1'b1; end
else begin
	ok <= 1'b0; end
end

assign i2s_out= (counter!= 5'b10000 && Wclk == 0) ? DLeft [counter] :
				(counter!= 5'b10000 && Wclk == 1) ? DRight [counter] :
				1'b0;
				
always @(negedge Bclk)
begin
    if (ok != last_ok)
    begin
        counter<=5'b10000;
        last_ok<=ok;
    end
    else
	if (counter != 5'b00000)
	begin 
		counter<=counter-1;
	end
end

endmodule