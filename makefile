SRC = src/
BIN = bin/
ARMTC = arm-none-eabi
KERNEL = kernel.bin
LIST = kernel.list
LINKER = kernel.ld
MAP = kernel.map
INCLUDE = $(SRC)include/
CFLAGS = -mcpu=arm926ej-s -O3 -marm -Wall -Wextra -nostdinc -nostdlib -nostartfiles -ffreestanding -std=gnu99

# main path
OBJECTS := $(patsubst $(SRC)%.S,$(BIN)%.o,$(wildcard $(SRC)*.S))
OBJECTS += $(patsubst $(SRC)%.c,$(BIN)%.o,$(wildcard $(SRC)*.c))
# mm
OBJECTS += $(patsubst $(SRC)mm/%.S,$(BIN)mm/%.o,$(wildcard $(SRC)mm/*.S))
OBJECTS += $(patsubst $(SRC)mm/%.c,$(BIN)mm/%.o,$(wildcard $(SRC)mm/*.c))

all: dirs $(KERNEL)
rebuild: all

dirs:
	@mkdir -p bin
	@mkdir -p bin/mm

$(KERNEL) : $(BIN)kernel.elf
	@echo MK  $@
	@$(ARMTC)-objcopy $(BIN)kernel.elf -O binary $(KERNEL) 

$(BIN)kernel.elf : $(OBJECTS) $(LINKER)
	@echo LD  $(LINKER)
	@$(ARMTC)-ld $(OBJECTS) -Map $(MAP) --no-undefined  -o $(BIN)kernel.elf -T $(LINKER)

$(BIN)%.o: $(SRC)%.S
	@echo AS  $<
	@$(ARMTC)-gcc -nostdinc -I $(INCLUDE) -E  -mcpu=arm926ej-s $< -o $@.s
	@$(ARMTC)-as -I $(INCLUDE) -mcpu=arm926ej-s $@.s -o $@

$(BIN)%.o: $(SRC)%.c
	@echo CC  $<
	@$(ARMTC)-gcc $(CFLAGS) -c -I $(INCLUDE) $< -o $@
	@$(ARMTC)-gcc $(CFLAGS) -S -c -I $(INCLUDE) $< -o $@.S

	
clean: 
	@rm -f $(BIN)*.o 
	@rm -f $(BIN)mm/*.o 
	@rm -f $(BIN)kernel.elf
	@rm -f $(KERNEL)
	@rm -f $(MAP)