##############################################################################
# Makefile for crc32_bootinfo
#
# File:		boot_info.mk
# Author:	Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# Date:		15 April 2025
# Version:	1.0
# Brief:	Builds a boot information hex file. Depends on application
# 			bin file to generate boot info such as file size, crc etc.
#
# Copyright (c) 2024-2025 Accolade Electronics Pvt. Ltd. All Rights Reserved.
#
# Changelog:
#
# 2025-04-15   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
#   - Initial creation of the Makefile for crc32_bootinfo project.
#
##############################################################################

SHELL = cmd.exe

TOOLCHAIN_PATH := C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-armllvm_4.0.1.LTS
CC := $(TOOLCHAIN_PATH)/bin/tiarmclang
#LD := $(TOOLCHAIN_PATH)/bin/tiarmlnk
LD := $(CC)
OBJCOPY := $(TOOLCHAIN_PATH)/bin/tiarmhex

OUTPUT_DIR := build
BOOTINFO_DIR := ../boot_info

APP_BIN := ti_firmware.bin
LINKER_SCRIPT :=$(BOOTINFO_DIR)/linker.cmd
BOOTINFO_SRC := boot_info_data.c
BOOTINFO_OBJ := $(OUTPUT_DIR)/boot_info_data.o
BOOTINFO_ELF := $(OUTPUT_DIR)/boot_info.out
BOOTINFO_HEX := $(OUTPUT_DIR)/boot_info.hex

DEVICE := MSPM0G3507
CPU_FLAGS := -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb
CFLAGS := -D__$(DEVICE)__ $(CPU_FLAGS) -O2 -Wall -Wextra
LDFLAGS := -Wl,-m"$(OUTPUT_DIR)/boot_info.map" \
		   -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections \
		   -Wl,--xml_link_info="$(OUTPUT_DIR)/boot_info_linkInfo.xml" \
		   -Wl,--rom_model -Wl,--entry_point=_dummy_entry -Wl,--disable_auto_rts


EXT := .exe
RM := DEL /F
RMDIR := RMDIR /S/Q

.PHONY: all clean

all: $(BOOTINFO_HEX)

$(OUTPUT_DIR):
	@if not exist "$(OUTPUT_DIR)" mkdir "$(OUTPUT_DIR)"

$(BOOTINFO_HEX): $(BOOTINFO_ELF)
	@echo 'Building HEX target: "$@"'
	@$(OBJCOPY) --memwidth=8 --romwidth=8 --diag_wrap=off --intel -o $@ $<
	@echo 'Finished building target: "$@"'
	@echo ' '

$(BOOTINFO_ELF): $(BOOTINFO_OBJ) $(LINKER_SCRIPT)
	@echo 'Building ELF target: "$@"'
	@$(LD) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo 'Finished building target: "$@"'
	@echo ' '

$(BOOTINFO_OBJ): $(BOOTINFO_SRC) | $(OUTPUT_DIR)
	@echo 'Compiling source: "$@"'
	@$(CC) $(CFLAGS) -o $@ -c $<
	@echo 'Finished compiling source: "$@"'
	@echo ' '

$(BOOTINFO_SRC): $(BOOTINFO_DIR)/generate_bootinfo_file$(EXT) $(APP_BIN)
	@echo 'Creating source file: "$@"'
	@$(BOOTINFO_DIR)/generate_bootinfo_file$(EXT) $(APP_BIN)
	@echo 'Finished creating file: "$@"'
	@echo ' '

clean:
	-@$(RMDIR) $(OUTPUT_DIR)
	-@$(RM) $(BOOTINFO_SRC)
