##############################################################################
# Makefile for ti_firmware Hex and Bin
#
# File:		hex_and_bin.mk
# Author:	Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# Date:		2 May 2025
# Brief:	Builds the ti_firmware.hex and ti_firmware.bin. Depends on
# 			the tiarmhex
#
# Copyright (c) 2024-2025 Accolade Electronics Pvt. Ltd. All Rights Reserved.
#
# Changelog:
#
# 2025-05-2   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
#   - Created hex_and_bin.mk file
#
##############################################################################


SHELL 	= cmd.exe

EXT		= .exe
RM 		= DEL /F
RMDIR 	= RMDIR /S/Q

TOOLCHAIN_PATH	:= C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-armllvm_4.0.1.LTS
CC 				:= $(TOOLCHAIN_PATH)/bin/tiarmclang$(EXT)
LD 				:= $(TOOLCHAIN_PATH)/bin/tiarmlnk$(EXT)
TOOL_HEX 		:= $(TOOLCHAIN_PATH)/bin/tiarmhex$(EXT)

OUTPUT_DIR := .

APP_ELF := ti_firmware.out
APP_HEX := $(OUTPUT_DIR)/ti_firmware.hex
APP_BIN := $(OUTPUT_DIR)/ti_firmware.bin

.PHONY: all clean

all: $(APP_HEX) $(APP_BIN)

$(OUTPUT_DIR):
	@if not exist "$(OUTPUT_DIR)" mkdir "$(OUTPUT_DIR)"

$(APP_BIN): $(APP_ELF) | $(OUTPUT_DIR)
	@echo 'Building Binary target: "$@"'
	$(TOOL_HEX) --diag_wrap=off --binary -o $@ $<
	@echo 'Finished building target: "$@"'
	@echo ' '

$(APP_HEX): $(APP_ELF) | $(OUTPUT_DIR)
	@echo 'Building HEX target: "$@"'
	$(TOOL_HEX) --memwidth=8 --romwidth=8 --diag_wrap=off --intel -o $@ $<
	@echo 'Finished building target: "$@"'
	@echo ' '

clean:
	-@$(RM) "$(APP_BIN)" "$(APP_HEX)"
