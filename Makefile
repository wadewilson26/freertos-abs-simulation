CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc

CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -g -Wall -fno-exceptions -fno-unwind-tables
INCLUDES = -I./include -I./freertos/include

LDFLAGS = -T scripts/qemu.ld -nostartfiles

SRC = src/main.c \
      src/startup.c \
      src/uart.c \
      freertos/list.c \
      freertos/queue.c \
      freertos/tasks.c \
      freertos/port.c \
      freertos/heap_4.c \
      freertos/timers.c

OBJ = $(SRC:.c=.o)
TARGET = build/abs_ecu.elf

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p build
	$(LD) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^
	@echo "[SUCCESS] Build complete: $@"

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f src/*.o freertos/*.o build/*
	@echo "[CLEAN] Build environment cleared."