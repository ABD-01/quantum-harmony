#**************************************************************************************************
#
#   Makefile for creating executable for post build pipelines
#	
#	Author:	Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
#   Date:	21 May, 2025
#   Copyright (c) 2024-2025 Accolade Electronics Pvt. Ltd. All Rights Reserved.
#
#
#**************************************************************************************************

SHELL      := /usr/bin/bash

CC_DEFAULT  := clang.exe
CXX_DEFAULT := clang++.exe

# Fallback to gcc/g++ if clang not found
CC  := "$(shell command -v clang.exe 2>/dev/null || command -v gcc)"
CXX := "$(shell command -v clang++.exe 2>/dev/null || command -v g++)"

# Error if no compiler found
ifndef CC
$(error No suitable C compiler found (clang or gcc))
endif

CFLAGS		= -Wall -Os -std=c99
CXXFLAGS	= -Wall -Os -std=c++17
MAKEFLAGS	+= --no-print-directory

CFLAGS		+= $(CUSTOM_CFLAGS)
CXXFLAGS	+= $(CUSTOM_CFLAGS)

OUTPUT_DIR	:= build
EXE_FILE	:= $(OUTPUT_DIR)/generate_bootinfo_file.exe
SRC_FILE	:= bootinfo_and_postbuild.cpp

.PHONY: all clean check

ifndef CC
$(error No suitable C compiler found (tried $(CC_DEFAULT) and gcc))
endif

all: $(EXE_FILE)

$(EXE_FILE): $(SRC_FILE) 
	@$(MAKE) check
	@mkdir -p $(OUTPUT_DIR)
	@echo "Compiling $< to $@..."
	@$(CXX) $(CXXFLAGS) -o $(EXE_FILE) $(SRC_FILE)
	@echo "Done."

check:
	@echo "Checking compiler version..."
	@$(CC) --version | head -n 1
	@echo

clean:
	@echo "Cleaning up..."
	-@rm -f $(OUTPUT_DIR)/*.exe
