# Toolchain definitions
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Compiler Flags: Cortex-M3 architecture, thumb instructions, no optimization, debugging on
CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -g -Wall -fno-exceptions -fno-unwind-tables 
# Tell the compiler where to look for .h files
CFLAGS += -I./include -I./freertos/include

# Linker Flags: Use our memory map, don't use default C startup code
LDFLAGS = -T scripts/qemu.ld -nostartfiles

# All the source files we need to compile
SRCS = src/main.c src/startup.c src/uart.c \
       freertos/list.c freertos/queue.c freertos/tasks.c \
       freertos/port.c freertos/heap_4.c freertos/timers.c

# Automatically replace .c with .o for our object files
OBJS = $(SRCS:.c=.o)

# The final output names
TARGET = build/abs_ecu

# Default command when you type 'make'
all: $(TARGET).elf $(TARGET).bin

# How to build the final .elf file
$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# How to build the individual .o object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# How to extract the raw binary from the .elf file
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Command to clean up compiled files
clean:
	rm -f src/*.o freertos/*.o build/*