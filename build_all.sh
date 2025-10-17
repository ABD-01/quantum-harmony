#!/bin/bash

# Build for default platform
echo
echo "Building for default platform..."
./build_module.sh default

# Build for Microchip MCU
echo
echo "Building for Microchip MCU platform..."
./build_module.sh mc

# Build for NXP MCU
echo
echo "Building for NXP MCU platform..."
./build_module.sh nxp

# Build for STM32
echo
echo "Building for STM32 platform..."
./build_module.sh st

# Build for TI MCU
echo
echo "Building for TI MCU platform..."
./build_module.sh ti

# Build for Renesas MCU
echo
echo "Building for Renesas MCU platform..."
./build_module.sh renesas
