
TARGET = iap_routine
CROSS = arm-none-eabi-

CC = $(CROSS)gcc
OC = $(CROSS)objcopy
OD = $(CROSS)objdump
RM = rm -f

CFLAGS  = -mthumb -mcpu=cortex-m0 -Iinclude
CFLAGS += -Wall -Wimplicit -Wcast-align -Wpointer-arith -Wswitch
CFLAGS += -nostdlib

LDFLAGS = -nostartfiles -T iap.ld -Wl,-Map=$(TARGET).map,--cref,--gc-sections -static

ASRC = iap.s

_AOBJ =  $(ASRC:.s=.o)
AOBJ = $(patsubst %, %,$(_AOBJ))

all: $(AOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET).elf $(AOBJ) $(COBJ)
	arm-none-eabi-objdump -D $(TARGET).elf > $(TARGET).dis

clean:
	rm -f *~ $(TARGET).* *.o
	
$(AOBJ) : %.o : %.s
	$(CC) $(CFLAGS) -c $< -o $@
