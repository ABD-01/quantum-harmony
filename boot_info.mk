##############################################################################
# Makefile for crc32_bootinfo
#
# File:		boot_info.mk
# Author:	Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# Date:		15 April 2025
# Version:	1.1
# Brief:	Builds a boot information hex file. Depends on application
# 			bin file to generate boot info such as file size, crc etc.
#
# Copyright (c) 2024-2025 Accolade Electronics Pvt. Ltd. All Rights Reserved.
#
# Changelog:
#
# 2025-04-23   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
#   - (v1.1) Added targets "ti_firmware_with_appcrc.hex", corresponding bin and 
#     required command files for srec_cat.exe.
#
# 2025-04-23   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
#   - Added target "boot_info.bin" to be created alongside the hex.
#
# 2025-04-15   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
#   - (v1.0) Initial creation of the Makefile for crc32_bootinfo project.
#
##############################################################################

SHELL 	= cmd.exe

EXT		= .exe
RM 		= DEL /F
RMDIR 	= RMDIR /S/Q

TOOLCHAIN_PATH	:= C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-armllvm_4.0.1.LTS
CC 				:= $(TOOLCHAIN_PATH)/bin/tiarmclang
#LD 			:= $(TOOLCHAIN_PATH)/bin/tiarmlnk
LD 				:= $(CC)
OBJCOPY 		:= $(TOOLCHAIN_PATH)/bin/tiarmhex

OUTPUT_DIR		:= build
BOOTINFO_DIR 	:= ../boot_info

POST_BUILD_UTIL	:= $(BOOTINFO_DIR)/generate_bootinfo_file$(EXT)
SREC_CAT 		:= srec_cat$(EXT)
SREC_CMP 		:= srec_cmp$(EXT)

APP_HEX			:= ti_firmware.hex
APP_BIN			:= ti_firmware.bin
LINKER_SCRIPT 	:= $(BOOTINFO_DIR)/linker.cmd

BOOTINFO_SRC 	:= boot_info_data.c
BOOTINFO_OBJ 	:= $(OUTPUT_DIR)/boot_info_data.o
BOOTINFO_ELF 	:= $(OUTPUT_DIR)/boot_info.out
BOOTINFO_HEX 	:= $(OUTPUT_DIR)/boot_info.hex
BOOTINFO_BIN 	:= $(OUTPUT_DIR)/boot_info.bin

CMD_FILE1	 	 := srec_cmd_create_hex_with_appcrc.txt
CMD_FILE2 		 := srec_cmd_create_bin_with_appcrc.txt
APP_CRC_BIN		 := app_crc32.bin
APP_HEX_WITH_CRC := ti_firmware_with_appcrc.hex
APP_BIN_WITH_CRC := ti_firmware_with_appcrc.bin

DEVICE		:= MSPM0G3507
CPU_FLAGS	:= -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb
CFLAGS 		:= -D__$(DEVICE)__ $(CPU_FLAGS) -O2 -Wall -Wextra
LDFLAGS 	:= -Wl,-m"$(OUTPUT_DIR)/boot_info.map" \
			   -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections \
			   -Wl,--xml_link_info="$(OUTPUT_DIR)/boot_info_linkInfo.xml" \
			   -Wl,--rom_model -Wl,--entry_point=_dummy_entry -Wl,--disable_auto_rts


ifeq (,$(wildcard $(POST_BUILD_UTIL)))
$(error "$(POST_BUILD_UTIL) not found")
endif

ifeq (,$(shell where $(SREC_CAT) 2>NUL))
$(error "$(SREC_CAT) not found. Install srec_cat or add to PATH if installed already.")
endif


.PHONY: all clean

all: $(BOOTINFO_HEX) $(BOOTINFO_BIN) $(APP_HEX_WITH_CRC) $(APP_BIN_WITH_CRC)

$(OUTPUT_DIR):
	@if not exist "$(OUTPUT_DIR)" mkdir "$(OUTPUT_DIR)"

$(BOOTINFO_BIN): $(BOOTINFO_ELF)
	@echo 'Building Binary target: "$@"'
	$(OBJCOPY) --diag_wrap=off --binary -o $@ $<
	@echo 'Finished building target: "$@"'
	@echo ' '

$(BOOTINFO_HEX): $(BOOTINFO_ELF)
	@echo 'Building HEX target: "$@"'
	$(OBJCOPY) --memwidth=8 --romwidth=8 --diag_wrap=off --intel -o $@ $<
	@echo 'Finished building target: "$@"'
	@echo ' '

$(BOOTINFO_ELF): $(BOOTINFO_OBJ) $(LINKER_SCRIPT)
	@echo 'Building ELF target: "$@"'
	$(LD) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo 'Finished building target: "$@"'
	@echo ' '

$(BOOTINFO_OBJ): $(BOOTINFO_SRC) | $(OUTPUT_DIR)
	@echo 'Compiling source: "$@"'
	$(CC) $(CFLAGS) -o $@ -c $<
	@echo 'Finished compiling source: "$@"'
	@echo ' '

$(BOOTINFO_SRC): $(APP_BIN) $(POST_BUILD_UTIL)
	@echo 'Creating source file: "$@"'
	$(POST_BUILD_UTIL) $<
	@echo 'Finished creating file: "$@"'
	@echo ' '

$(APP_BIN_WITH_CRC): $(APP_HEX) $(APP_CRC_BIN) $(CMD_FILE2)
	@echo 'Creating BIN file with CRC: "$@"'
	$(SREC_CAT) $< @$(CMD_FILE2) $@ -Binary
	@echo 'Finished creating file: "$@"'
	@echo 'Comparing files: "$(APP_BIN)" and "$@"'
	@$(SREC_CMP) $(APP_BIN) -Binary $@ -Binary -Verbose || true
	@echo ' '


$(APP_HEX_WITH_CRC): $(APP_HEX) $(APP_CRC_BIN) $(CMD_FILE1)
	@echo 'Creating HEX file with CRC: "$@"'
	$(SREC_CAT) $< @$(CMD_FILE1) $@ -Intel
	@echo 'Finished creating file: "$@"'
	@echo ' '

clean:
	-@$(RMDIR) $(OUTPUT_DIR)
	-@$(RM) $(BOOTINFO_SRC) $(APP_CRC_BIN) $(APP_HEX_WITH_CRC) $(APP_BIN_WITH_CRC) $(CMD_FILE1) $(CMD_FILE2)
