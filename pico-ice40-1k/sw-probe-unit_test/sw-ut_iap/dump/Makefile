
all: convert do_convert dis

convert:
	gcc -o convert convert.c

do_convert:
	./convert >iap.bin

dis:
	arm-none-eabi-objdump -b binary --adjust-vma=0x1FFF1000 -m arm -Mforce-thumb -D iap.bin > iap.dis

clean:
	rm -f convert iap.bin iap.dis
	rm -f *~
