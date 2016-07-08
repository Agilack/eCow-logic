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
 #1134 Wclk <= 1'b1;
 #1134 Wclk <= 1'b0;
end

always 
begin
	#7 Bclk <= 1'b1;
	#7 Bclk <= 1'b0;
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

wire ram_oe;
wire ram_ce;
wire ram_we;
wire  [18:0] ram_a;
wire [ 7:0] ram_dq;

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
  .i2c_sda_io(sda),
  .ram_oe_o   ( ram_oe      ),
    .ram_ce_o   ( ram_ce      ),
    .ram_we_o   ( ram_we      ),
    .ram_dq_io  ( ram_dq[7:0] ),
    .ram_a_o    ( ram_a[18:0] )
);

initial begin
	sim_clk=0;
	cs=1;
	mosi=1;
	sck=0;

end
assign but[3:0]=4'b1010;

reg [7:0] plante;
assign ram_dq[7:0] = plante [7:0];

always @(posedge sim_clk)
begin
	plante[7:0] <= ram_a[7:0];
end

// assign wb_we =(state==STATE_RAM) ? 1'b1 : 1'b0;
// assign ram_dq[7:0] = (ram_a == 19'd0) ? 8'hAA : 8'h55;
						/*(ram_a [18:0 == 19'd1]) ? 8'hBB :
						(ram_a [18:0 == 19'd2]) ? 8'h12 :
						(ram_a [18:0 == 19'd3]) ? 8'h62 :
						(ram_a [18:0 == 19'd4]) ? 8'h25 :
						(ram_a [18:0 == 19'd5]) ? 8'h44 :
						(ram_a [18:0 == 19'd6]) ? 8'h65 :
						(ram_a [18:0 == 19'd7]) ? 8'h33 :
						(ram_a [18:0 == 19'd8]) ? 8'hCC :
						(ram_a [18:0 == 19'd9]) ? 8'h95 :
						(ram_a [18:0 == 19'd10]) ? 8'h32 :
						(ram_a [18:0 == 19'd11]) ? 8'h41 :
						(ram_a [18:0 == 19'd12]) ? 8'h98 :
						(ram_a [18:0 == 19'd13]) ? 8'hA0 :
						(ram_a [18:0 == 19'd14]) ? 8'hB9 :
						(ram_a [18:0 == 19'd15]) ? 8'hC2 :
						(ram_a [18:0 == 19'd16]) ? 8'hE5 :
						(ram_a [18:0 == 19'd17]) ? 8'hF6 :
						(ram_a [18:0 == 19'd18]) ? 8'hC2 :
						(ram_a [18:0 == 19'd19]) ? 8'hD4 :
						(ram_a [18:0 == 19'd20]) ? 8'hB2 :
						(ram_a [18:0 == 19'd21]) ? 8'h6d :
						(ram_a [18:0 == 19'd22]) ? 8'h9D :
						(ram_a [18:0 == 19'd23]) ? 8'h8A :
						(ram_a [18:0 == 19'd24]) ? 8'h51 :
						(ram_a [18:0 == 19'd25]) ? 8'h68 :
						(ram_a [18:0 == 19'd26]) ? 8'h95 :
						(ram_a [18:0 == 19'd27]) ? 8'h12 :
						(ram_a [18:0 == 19'd28]) ? 8'h64 :
						8'hFF;
						*/
initial begin

	#2 cs=0;
	#200 mosi_octet (8'hC0); // I2C mode
	#200 mosi_octet (8'h0A);
	#200 mosi_octet (8'h5A);
	#200 mosi_octet (8'h50);
	#200 mosi_octet (8'hF3);
	#200 mosi_octet (8'h55);
	#2 cs=1;
	
	#500000 cs=0;
	#200 mosi_octet (8'h40); // Sram mode
	#200 mosi_octet (8'h0A);
	#200 mosi_octet (8'h5A);
	#200 mosi_octet (8'h50);
	#200 mosi_octet (8'hF3);
	#200 mosi_octet (8'h55);
		#200 mosi_octet (8'h0A);
	#200 mosi_octet (8'h5A);
	#200 mosi_octet (8'h50);
	#200 mosi_octet (8'hF3);
	#200 mosi_octet (8'h55);
		#200 mosi_octet (8'h0A);
	#200 mosi_octet (8'h5A);
	#200 mosi_octet (8'h50);
	#200 mosi_octet (8'hF3);
	#200 mosi_octet (8'h55);
		#200 mosi_octet (8'h0A);
	#200 mosi_octet (8'h5A);
	#200 mosi_octet (8'h50);
	#200 mosi_octet (8'hF3);
	#200 mosi_octet (8'h55);
		#200 mosi_octet (8'h0A);
	#200 mosi_octet (8'h5A);
	#200 mosi_octet (8'h50);
	#200 mosi_octet (8'hF3);
	#200 mosi_octet (8'h55);
		#200 mosi_octet (8'h0A);
	#200 mosi_octet (8'h5A);
	#200 mosi_octet (8'h50);
	#200 mosi_octet (8'hF3);
	#200 mosi_octet (8'h55);
		#200 mosi_octet (8'h0A);
	#200 mosi_octet (8'h5A);
	#200 mosi_octet (8'h50);
	#200 mosi_octet (8'hF3);
	#200 mosi_octet (8'h55);
		#200 mosi_octet (8'h0A);
	#200 mosi_octet (8'h5A);
	#200 mosi_octet (8'h50);
	#200 mosi_octet (8'hF3);
	#200 mosi_octet (8'h55);
	#2 cs=1;
	// wait state IDLLE
	#2 cs=0;
	#500000 mosi_octet(8'h00); //I2S mode
	#2 cs=1;

	#20000;
	
	//#200 mosi_octet (8'hC0); // i2c mode trop top pour le remettre

	

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
		
	end
endtask



initial
begin
$dumpfile("test.vcd");
$dumpvars(1,out_dac, uut);

#2000000 $finish;
end

endmodule