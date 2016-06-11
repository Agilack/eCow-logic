
TARGET = ut_reloc.bin

all:
	make -C boot
	make -C firmware
	cp  boot/ut_reloc_boot.bin $(TARGET)
	cat firmware/ut_reloc_firmware.bin >> $(TARGET)

clean:
	rm -f $(TARGET) *~
	make -C boot clean
	make -C firmware clean
