
TARGET = ut_uart
CROSS = arm-none-eabi-

CC = $(CROSS)gcc
OC = $(CROSS)objcopy
RM = rm -f

THUMB    = -mthumb
THUMB_IW = -mthumb-interwork

CFLAGS  = -g -O3 -mthumb -mcpu=cortex-m0 -DCORTEX_M0 -DUSE_STDPERIPH_DRIVER -Iinclude

all:
	$(CC) $(CFLAGS)  -c -o startup_W7500.o startup_W7500.S
	$(CC) $(CFLAGS)  -c -o system_W7500x.o system_W7500x.c
	$(CC) $(CFLAGS)  -c -o W7500x_it.o     W7500x_it.c
	$(CC) $(CFLAGS)  -c -o W7500x_gpio.o   W7500x_gpio.c
	$(CC) $(CFLAGS)  -c -o W7500x_uart.o   W7500x_uart.c
	$(CC) $(CFLAGS)  -c -o main.o          main.c
	$(CC) $(CFLAGS) -T gcc_W7500.ld -o $(TARGET).elf startup_W7500.o system_W7500x.o main.o W7500x_it.o W7500x_gpio.o W7500x_uart.o

	$(OC) -S $(TARGET).elf -O binary $(TARGET).bin

clean:
	$(RM) $(TARGET).elf $(TARGET).bin
	$(RM) *.o
	$(RM) *~
