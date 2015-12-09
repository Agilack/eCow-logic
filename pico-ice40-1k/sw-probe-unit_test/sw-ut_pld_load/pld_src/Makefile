all:
	@echo "Synthesis ..."
	@yosys -p "read_verilog rtl/ut.v; synth_ice40 -blif ut.blif" > log-1-synth 2>&1
	@echo "Place and Route ..."
	@arachne-pnr -d 1k -p rtl/ut.pcf -o ut.txt ut.blif > log-2-pnr 2>&1
	@echo "Generate BIN ..."
	@icepack ut.txt bitstream.bin > log-3-bin 2>&1

clean:
	rm -f ut.blif ut.txt *.bin
	rm -f log-* *~