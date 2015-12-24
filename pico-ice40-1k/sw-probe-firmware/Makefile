##
 # eCow-logic - Embedded probe main firmware
 #
 # Copyright (c) 2015 Saint-Genest Gwenael <gwen@agilack.fr>
 #
 # This file may be distributed and/or modified under the terms of the
 # GNU General Public License version 2 as published by the Free Software
 # Foundation. (See COPYING.GPL for details.)
 #
 # This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 # WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 #
TARGET = probe-fw
CROSS = arm-none-eabi-

CC = $(CROSS)gcc
OC = $(CROSS)objcopy
OD = $(CROSS)objdump
RM = rm -f

CFLAGS  = -mthumb -mcpu=cortex-m0 -Iinclude
CFLAGS += -Wall -Wimplicit -Wcast-align -Wpointer-arith -Wswitch
CFLAGS += -nostdlib

LDFLAGS = -nostartfiles -T gcc_W7500.ld -Wl,-Map=$(TARGET).map,--cref,--gc-sections -static

SRC  = main.c hardware.c api.c libc.c uart.c
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
	$(RM) $(TARGET).elf $(TARGET).bin $(TARGET).map $(TARGET).dis
	$(RM) *.o
	$(RM) *~ include/*~

$(AOBJ) : %.o : %.s
	@echo "  [AS] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

$(COBJ) : %.o : %.c
	@echo "  [CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@
