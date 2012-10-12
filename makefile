
SRC = src/
BIN = bin/
ARMTC = arm-none-eabi
KERNEL = kernel.bin
LIST = kernel.list
LINKER = kernel.ld
MAP = kernel.map


OBJECTS := $(patsubst $(SRC)%.S,$(BIN)%.o,$(wildcard $(SRC)*.S))
OBJECTS += $(patsubst $(SRC)%.c,$(BIN)%.o,$(wildcard $(SRC)*.c))

all: $(KERNEL)
rebuild: all

$(KERNEL) : $(BIN)kernel.elf
	@echo MK  $@
	@$(ARMTC)-objcopy $(BIN)kernel.elf -O binary $(KERNEL) 

$(BIN)kernel.elf : $(OBJECTS) $(LINKER)
	@echo LD  $(LINKER)
	@$(ARMTC)-ld $(OBJECTS) -Map $(MAP) --no-undefined  -o $(BIN)kernel.elf -T $(LINKER)

$(BIN)%.o: $(SRC)%.S
	@echo AS  $<
	@$(ARMTC)-gcc -I $(SRC) -E -mcpu=arm926ej-s $< -o $@.s
	@$(ARMTC)-as -I $(SRC) -mcpu=arm926ej-s $@.s -o $@

$(BIN)%.o: $(SRC)%.c
	@echo CC  $<
	@$(ARMTC)-gcc -mcpu=arm926ej-s -O3 -Wall -Wextra -nostdlib -nostartfiles -ffreestanding -std=gnu99 -c -I $(SRC) $< -o $@
#	@$(ARMTC)-gcc -mcpu=arm926ej-s -O3 -S -Wall -Wextra -nostdlib -nostartfiles -ffreestanding -std=gnu99 -c -I $(SRC) $< -o $@.S

	
clean: 
	@rm -f $(BIN)*.o 
	@rm -f $(BIN)kernel.elf
	@rm -f $(KERNEL)
	@rm -f $(MAP)