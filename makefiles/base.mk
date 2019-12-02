ifeq ($(ARCH),umps)
include makefiles/umps.mk
else ifeq ($(ARCH),uarm)
include makefiles/uarm.mk
endif

# Toolchain
CC := $(ARCH_PREFIX)gcc
LD := $(ARCH_PREFIX)ld

# Directories
SRC_DIR := ./src
INCLUDE_DIR := ./include
BUILD_DIR := ./out/$(ARCH)

# Sources
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS)) 
OBJS += $(addprefix $(BUILD_DIR)/,$(ARCH_OBJS))

# Compiler options
CFLAGS := $(ARCH_CFLAGS) -I. -I$(ARCH_DIR) -I$(INCLUDE_DIR) -Wall -O0

# Linker options
LDFLAGS := -G 0 -nostdlib $(ARCH_LDFLAGS)

VPATH := $(ARCH_DIR)

# Executables
TARGET := kernel.core.$(ARCH)
KERNEL := $(BUILD_DIR)/kernel

all: $(TARGET)

$(TARGET): $(KERNEL)
	$(ARCH_OBJCPY) -k $<

$(KERNEL): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
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