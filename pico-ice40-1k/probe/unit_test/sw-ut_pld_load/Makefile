
TARGET = ut_pld_load
CROSS = arm-none-eabi-
#CROSS=arm-none-linux-gnueabi-
FW_BIN = pld_src/bitstream.bin

CC = $(CROSS)gcc
OC = $(CROSS)objcopy
OD = $(CROSS)objdump
RM = rm -f

CFLAGS  = -mthumb -mcpu=cortex-m0 -Iinclude
CFLAGS += -Wall -Wimplicit -Wcast-align -Wpointer-arith -Wswitch
CFLAGS += -nostdlib

LDFLAGS = -nostartfiles -T gcc_W7500.ld -Wl,-Map=$(TARGET).map,--cref,--gc-sections -static

SRC  = main.c hardware.c uart.c
SRC += spi.c  pld.c
ASRC = startup.s

_COBJ =  $(SRC:.c=.o)
_AOBJ =  $(ASRC:.s=.o)
COBJ = $(patsubst %, %,$(_COBJ))
AOBJ = $(patsubst %, %,$(_AOBJ))
FW_OBJ = bitstream.o

all: $(AOBJ) $(COBJ) $(FW_OBJ)
	@echo "  [LD] $(TARGET).elf"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET).elf $(AOBJ) $(COBJ) $(FW_OBJ)
	@echo "  [OC] $(TARGET).bin"
	@$(OC) -S $(TARGET).elf -O binary $(TARGET).bin
	@echo "  [OD] $(TARGET).dis"
	@$(OD) -D $(TARGET).elf > $(TARGET).dis

clean:
	$(RM) $(TARGET).elf $(TARGET).bin $(TARGET).map $(TARGET).dis
	$(RM) *.o
	$(RM) *~
	@make -C pld_src clean

$(AOBJ) : %.o : %.s
	@echo "  [AS] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

$(COBJ) : %.o : %.c
	@echo "  [CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

$(FW_OBJ):
	@echo "[MAKE] pld_src subdir"
	@make --quiet -C pld_src
	@echo "  [OC] $@"
	@$(OC) -I binary -O elf32-littlearm -B arm $(FW_BIN) $(FW_OBJ)
