`timescale 10ns/1ns

module tb
();

reg sim_clk;
reg clk_100k;

always
begin
  #2 sim_clk <= 1'b1;
  #2 sim_clk <= 1'b0;
end

always
begin
  #50 clk_100k <= 1'b1;
  #50 clk_100k<= 1'b0;
end

wire SDA;
wire SCL;
reg mosi;
reg [2:0] out_dac;

reg[7:0] DFromGLU;
wire[7:0] DtoGLU;
reg reset;

assign SDA=(out_dac==1) ? 1'b1 :(out_dac==2) ? 1'b0 : 1'bz;

reg RxReady;

i2c uut
(
	.clk(sim_clk),
	
  .clk_100k(clk_100k),
  .i2c_sda_io(SDA),
  .i2c_scl(SCL),
  .DIN(DFromGLU),
  .DOUT(DToGLU),
  .reset(reset),
  .copy_enable(RxReady)
);

initial begin
	RxReady=0;
end
    
initial begin
	#20 reset=1;
	#2 reset=0;
	#2 DFromGLU=8'h70;
	#5 RxReady=1;
	#1 RxReady=0;
	#200 DFromGLU=8'h25;
	#5 RxReady=1;
	#1 RxReady=0;
	#200 DFromGLU=8'hAA;
	#5 RxReady=1;
	#1 RxReady=0;
	#4000 DFromGLU=8'h71;
	#1 RxReady=1;
	#1 RxReady=0;
	#200 DFromGLU=8'h42;
	#1 RxReady=1;
	#1 RxReady=0;
	
end

initial
begin
#1 out_dac=0;
#4953 out_dac=1;
#10 out_dac=2;
#10 out_dac=1;
#10 out_dac=2;
#10 out_dac=1;
#19 out_dac=2;
#19 out_dac=1;
#1 out_dac=0;
end


initial
begin
$dumpfile("test.vcd");
$dumpvars(1, uut);

#1000000 $finish;
end

endmodule