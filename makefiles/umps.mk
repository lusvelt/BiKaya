ARCH_DIR := ./umps
ARCH_PREFIX := mipsel-linux-gnu-
ARCH_OBJCPY := umps2-elf2umps
ARCH_OBJS := crtso.o libumps.o
ARCH_CFLAGS := -ffreestanding -ansi -DTARGET_UMPS=1 -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32
ARCH_LDFLAGS := -T $(ARCH_DIR)/umpscore.ldscript