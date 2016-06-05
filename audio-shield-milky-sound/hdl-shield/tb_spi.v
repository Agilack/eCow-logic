`timescale 10ns/1ns

module tb
();

reg sim_clk;

always
begin
  #2 sim_clk <= 1'b1;
  #2 sim_clk <= 1'b0;
end


wire miso;
reg mosi;
reg cs;
reg sck;
wire[7:0] DFromSPI;
wire[7:0] DtoSPI;

wire scl;
wire RxReady;

spi uut
(
 .clk_i(sim_clk),
 .spi_miso_o(miso),
 .spi_mosi_i(mosi),
 .spi_sck_i(sck),
 .spi_cs_i(cs),
 .writeGlu(DFromSPI),
 .readGlu(DtoSPI),
 .newOctet(RxReady)
);

initial begin
	sim_clk=0;
	cs=1;
	mosi=1;
	sck=0;
	
end
    
initial begin
	#200
	#20 cs=0;
	#1 mosi=1;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=0;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=0;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#2 cs=1;
	#198
	#20 cs=0;
	#1 mosi=0;
	#1 sck=1;
	#50 mosi=0;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=0;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
		#1 sck=1;
	#50 mosi=1;
	#1 sck=0;
	#20 cs=1;
end




initial
begin
$dumpfile("test.vcd");
$dumpvars(1, uut);

#1000000 $finish;
end

endmodule