ARCH := uarm
ARCH_PREFIX := arm-none-eabi-
ARCH_OBJCPY := elf2uarm
ARCH_OBJS := crtso.o libuarm.o
ARCH_CFLAGS := -mcpu=arm7tdmi -DTARGET_UARM=1
ARCH_LDSCRIPT := elf32ltsarm.h.uarmcore.x

include makefiles/base.mk