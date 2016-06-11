
TARGET = ut_net
CROSS = arm-none-eabi-

CC = $(CROSS)gcc
OC = $(CROSS)objcopy
OD = $(CROSS)objdump
RM = rm -f

THUMB    = -mthumb
THUMB_IW = -mthumb-interwork

CFLAGS  = -mthumb -mcpu=cortex-m0 -DCORTEX_M0 -DUSE_STDPERIPH_DRIVER -Iinclude
CFLAGS += -Wall -Wimplicit -Wcast-align -Wpointer-arith -Wswitch
CFLAGS += -nostdlib
#CFLAGS += -nostartfiles -nostdlib -nostdinc -nodefaultlibs

LDFLAGS = -nostartfiles -T gcc_W7500.ld -Wl,-Map=$(TARGET).map,--cref,--gc-sections -static

SRC  = main.c hardware.c uart.c libc.c
#SRC += system_W7500x.c
#SRC += wizchip_conf.c
SRC += miim.c W7500x_wztoe.c
ASRC = startup.s

_COBJ =  $(SRC:.c=.o)
_AOBJ =  $(ASRC:.s=.o)
COBJ = $(patsubst %, %,$(_COBJ))
AOBJ = $(patsubst %, %,$(_AOBJ))

all: $(AOBJ) $(COBJ)
	@echo "  [LD] $(TARGET).elf"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET).elf $(AOBJ) $(COBJ)
	@echo "  [OC] $(TARGET).bin"
	@$(OC) -S $(TARGET).elf -O binary $(TARGET).bin
	@echo "  [OD] $(TARGET).dis"
	@$(OD) -D $(TARGET).elf > $(TARGET).dis

clean:
	$(RM) $(TARGET).elf $(TARGET).bin $(TARGET).dis $(TARGET).map
	$(RM) *.o
	$(RM) *~

$(AOBJ) : %.o : %.s
	@echo "  [AS] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

$(COBJ) : %.o : %.c
	@echo "  [CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@
