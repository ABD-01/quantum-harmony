##############################################################################
# Makefile for crc32_bootinfo
#
# File:		boot_info.mk
# Author:	Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# Date:		14 April 2025
# Version:	1.0
# Brief:	
#
# Copyright (c) 2024-2025 Accolade Electronics Pvt. Ltd. All Rights Reserved.
#
# Changelog:
#
# 2025-04-15   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
#   - Initial creation of the Makefile for crc32_bootinfo project.
#
##############################################################################

TOOLCHAIN_PATH := "C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-armllvm_4.0.1.LTS"

CC := $(TOOLCHAIN_PATH)/bin/tiarmclang
LD := $(TOOLCHAIN_PATH)/bin/tiarmlnk
OBJCOPY := $(TOOLCHAIN_PATH)/bin/tiarmhex

DDEVICE := -D__MSPM0G3507__
___FLAGS := -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb
CFLAGS := -O2

APP_BIN := ti_firmware.out
TARGET = ti_boot_info
BOOT_INFO_FILE = boot_info_data.c

OUTPUT_DIR := boot_info

.PHONY: all clean

all: $(OUTPUT_DIR)/$(TARGET).hex

$(OUTPUT_DIR)/$(TARGET).hex: $(OUTPUT_DIR)/$(TARGET).out


$(OUTPUT_DIR)/$(TARGET).out: $(BOOT_INFO_FILE)


$(BOOT_INFO_FILE): $(APP_BIN)
