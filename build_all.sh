#!/bin/bash

# Build for default platform
echo "Building for default platform..."
./build.sh default

# Build for Microchip MCU
echo "Building for Microchip MCU platform..."
./build.sh mc

# Build for NXP MCU
echo "Building for NXP MCU platform..."
./build.sh nxp

# Build for STM32
echo "Building for STM32 platform..."
./build.sh st

# Build for TI MCU
echo "Building for TI MCU platform..."
./build.sh ti
