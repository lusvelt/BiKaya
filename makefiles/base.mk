# Toolchain
CC := $(ARCH_PREFIX)gcc
LD := $(ARCH_PREFIX)ld

# Directories
SRC_DIR := ./src
INCLUDE_DIR := ./include
ARCH_DIR := ./$(ARCH)
BUILD_DIR := ./out/$(ARCH)
TEST_DIR := ./tests

# Sources
SRCS := $(wildcard $(SRC_DIR)/*.c)
ifdef TEST
	SRCS += $(TEST_DIR)/p2test_bikaya_v0.2.c
endif

# Objects
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
ifdef TEST
	OBJS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o, $(OBJS))
endif
OBJS += $(addprefix $(BUILD_DIR)/,$(ARCH_OBJS))

# Compiler options
CFLAGS := $(ARCH_CFLAGS) -I. -I$(ARCH_DIR) -I$(INCLUDE_DIR) -Wall -O0

# Linker options
LDFLAGS := -G 0 -nostdlib -T $(ARCH_DIR)/$(ARCH_LDSCRIPT)

VPATH := $(ARCH_DIR)

# Executables
TARGET := kernel.core.$(ARCH)
KERNEL := $(BUILD_DIR)/kernel

.PHONY: all clean print

all: $(TARGET)

$(TARGET): $(KERNEL)
	$(ARCH_OBJCPY) -k $<

$(KERNEL): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(ARCH_DIR)/%.s
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(ARCH_DIR)/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

# That's an awesome trick to avoid invoking `mkdir` if BUILD_DIR already exists
# See https://www.gnu.org/software/make/manual/html_node/Prerequisite-Types.html
# for an explanation of how it works.
$(OBJS): | $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $@

clean:
	-rm -rf $(BUILD_DIR)