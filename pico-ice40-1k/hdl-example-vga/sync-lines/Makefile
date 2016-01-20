all:
	@mkdir -p build
	@echo "Synthesis ..."
	@yosys -s yosys.sy > build/log-1-synth 2>&1
	@echo "Place and Route ..."
	@arachne-pnr -d 1k -p syn/ecow.pcf -o build/sync_lines.pnr build/sync_lines.blif > build/log-2-pnr 2>&1
	@echo "Generate BIN ..."
	@icepack build/sync_lines.pnr bitstream.bin > build/log-3-bin 2>&1

clean:
	rm -f build/sync_lines.blif build/sync_lines.pnr build/*.bin *.bin
	rm -f build/log-* *~