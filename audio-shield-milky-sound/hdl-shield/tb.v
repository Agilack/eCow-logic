`timescale 10ns/1ns

module tb
();

reg sim_clk;

always
begin
  #2 sim_clk <= 1'b1;
  #2 sim_clk <= 1'b0;
end

always
begin
 #1040 Wclk <= 1'b1;
 #1040 Wclk <= 1'b0;
end

always 
begin
	#26 Bclk <= 1'b1;
	#26 Bclk <= 1'b0;
end

wire miso;
reg mosi;
reg cs;
reg sck;

wire scl;
wire sda;
reg [2:0] out_dac=2'b0;
assign sda =(out_dac==1) ? 1'b1 :(out_dac==2) ? 1'b0 : 1'bz;

wire[7:0] led;
wire [3:0] but;
wire select;
wire micbias;
wire ToDAC;
wire Mclk;
reg Wclk;
reg Bclk;

top uut
(
  .clk(sim_clk),
  
  .led_0_o(led[0]),
  .led_1_o(led[1]),
  .led_2_o(led[2]),
  .led_3_o(led[3]),
  .led_4_o(led[4]),
  .led_5_o(led[5]),
  .led_6_o(led[6]),
  .led_7_o(led[7]),
  // Push Buttons
  .but_1_i(but[0]),
  .but_2_i(but[1]),
  .but_3_i(but[2]),
  .but_4_i(but[3]),
  
  //SPI Stuff
  .spi_miso(miso),
  .spi_mosi(mosi),
  .spi_sck(sck),
  .spi_cs(cs),
  
  .spi_select(select),
  .micbias(micbias),
  
  .DToDAC(ToDAC),
  .Mclk (Mclk),
  .Wclk(Wclk),
  .Bclk(Bclk),
  
  .i2c_scl(scl),
  .i2c_sda_io(sda)
);

initial begin
	sim_clk=0;
	cs=1;
	mosi=1;
	sck=0;
	
	
end
assign but[3:0]=4'b1010;
    
initial begin
	#200 mosi_octet (8'hC0); // i2c mode
	#200 mosi_octet (8'h30);
	#200 mosi_octet (8'h4b);
	#200 mosi_octet (8'hFF);
	
	#40000;
	
	//#200 mosi_octet (8'hC0); // i2c mode trop top pour le remettre
	#200 mosi_octet (8'h71);
	#200 mosi_octet (8'h25);
	

end



initial begin
	#1 out_dac =0;
	#920 out_dac =1;
	//#34920 out_dac =1;
	/*#100 out_dac =2;
	#200 out_dac=1;
	#300 out_dac=2;
	#200 out_dac=1;
	#100 out_dac =2;
	#200 out_dac=1;
	#200 out_dac=2;
	#200 out_dac=1;
	#185 out_dac=2; // don't think it'll be okay with HW
	#1 out_dac=0;*/
end

task mosi_octet;
	input [7:0] Byte;
	begin
		#20 cs=0;
		// #1 mosi=Byte[7];
		
		#50 mosi=Byte[7];
		 #1 sck=0;
		#1 sck=1;
		#50 mosi=Byte[6];
		#1 sck=0;
		#1 sck=1;
		#50 mosi=Byte[5];
		#1 sck=0;
		#1 sck=1;
		#50 mosi=Byte[4];
		#1 sck=0;
		#1 sck=1;
		#50 mosi=Byte[3];
		#1 sck=0;
		#1 sck=1;
		#50 mosi=Byte[2];
		#1 sck=0;
		#1 sck=1;
		#50 mosi=Byte[1];
		#1 sck=0;
		#1 sck=1;
		#50 mosi=Byte[0];
		#1 sck=0;
			#1 sck=1;
			#50 sck = 0;
		#2 cs=1;
	end
endtask

initial
begin
$dumpfile("test.vcd");
$dumpvars(1,out_dac, uut);

#1000000 $finish;
end

endmodule