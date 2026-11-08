TARGET_DIR = build

SRCS = $(wildcard drivers/*/src/*.c) main.c vendor/stm32/src/system_stm32f4xx.c
ASM_SRC = vendor/stm32/startup/startup_stm32f439xx.s
INC_DIRS = $(wildcard drivers/*/inc/) $(wildcard vendor/*/inc/)
OBJS = $(patsubst %.c, $(TARGET_DIR)/%.o, $(SRCS))
ASM_OBJ = $(patsubst %.s, $(TARGET_DIR)/%.o, $(ASM_SRC))
DEPS = $(patsubst %.o, %.d, $(OBJS))

-include $(DEPS)

CC = arm-none-eabi-gcc
GDB = gdb-multiarch

CFLAGS = -std=c11 -mthumb -mcpu=cortex-m4 -Wall -Wextra -Wpedantic -Og -g3
CFLAGS += $(addprefix -I,$(INC_DIRS)) -MMD -MP
CFLAGS += -DSTM32F439xx
LDFLAGS = -mthumb -mcpu=cortex-m4 -nostdlib
LDFLAGS += -T vendor/stm32/linker/stm32f439zitx_FLASH.ld

TARGET_MAIN = $(TARGET_DIR)/firmware.elf

.PHONY: all flash debug-server debug clean rebuild

all: $(TARGET_MAIN)

$(TARGET_MAIN): $(OBJS) $(ASM_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

$(TARGET_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) -mthumb -mcpu=cortex-m4 -c $< -o $@

flash: all
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
	-c "program $(TARGET_MAIN) verify reset exit"

debug-server:
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

debug: all
	$(GDB) -ex "target extended-remote localhost:3333" $(TARGET_MAIN)

clean:
	rm -rf $(TARGET_DIR)

rebuild: clean all