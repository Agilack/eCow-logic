all:
	@mkdir -p build
	@echo "Synthesis ..."
	@yosys -p "read_verilog rtl/top.v; synth_ice40 -blif build/spi_leds.blif" > build/log-1-synth 2>&1
	@echo "Place and Route ..."
	@arachne-pnr -d 1k -p syn/ecow.pcf -o build/spi_leds.pnr build/spi_leds.blif > build/log-2-pnr 2>&1
	@echo "Generate BIN ..."
	@icepack build/spi_leds.pnr bitstream.bin > build/log-3-bin 2>&1

clean:
	rm -f build/spi_leds.blif build/spi_leds.pnr build/*.bin *.bin
	rm -f build/log-* *~