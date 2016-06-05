

module SB_IO (

		inout PACKAGE_PIN,
		input OUTPUT_ENABLE,
		input D_OUT_0,
		input D_IN_0

);
	
	parameter PULLUP=1'b0;
	parameter [5:0] PIN_TYPE=6'b101001;
	
	assign PACKAGE_PIN = (OUTPUT_ENABLE==1) ? D_OUT_0 : D_IN_0; 
endmodule