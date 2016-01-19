all:
	@mkdir -p build
	@echo "Synthesis ..."
	@yosys -p "read_verilog rtl/k2000.v; synth_ice40 -blif build/k2000.blif" > build/log-1-synth 2>&1
	@echo "Place and Route ..."
	@arachne-pnr -d 1k -p syn/ecow.pcf -o build/k2000.pnr build/k2000.blif > build/log-2-pnr 2>&1
	@echo "Generate BIN ..."
	@icepack build/k2000.pnr bitstream.bin > build/log-3-bin 2>&1

clean:
	rm -f build/k2000.blif build/k2000.pnr build/*.bin *.bin
	rm -f build/log-* *~