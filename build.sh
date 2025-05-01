#!/bin/bash


# Step 1: Set toolchain options.
echo "***** Toolchain preparation *****"
# Clean previous.
sh clean.sh
# Compile flags.
export CFLAGS="-Wall -Werror -std=c99 -pedantic"
# Compiler toolchain selection.
if [ "${1}" = "stm32" ]; then
    echo "STM32 MCU compiler."
    export PREFIX="${1}"
    export PATH="${PATH}:/d/ST/STM32CubeIDE_1.16.0/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.12.3.rel1.win32_1.0.200.202406191623/tools/bin"
    export CC=arm-none-eabi-gcc
    export AR=arm-none-eabi-ar
    # Extra compiler flags taken from 'SAMPARK' project.
    export ECFLAGS="-mcpu=cortex-m33 -g3 -Og -ffunction-sections -fdata-sections -fstack-usage -fcyclomatic-complexity --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb"
elif [ "${1}" = "ti" ]; then
    echo "Using TI MCU compiler."
    export PREFIX="${1}"
    export PATH="${PATH}:/d/ti/ccs2002/ccs/tools/compiler/ti-cgt-armllvm_4.0.1.LTS/bin"
    export CC=tiarmclang
    export AR=tiarmar
    # Extra compiler flags taken from 'A2T' project.
    export ECFLAGS="-march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -gdwarf-3"
elif [ "${1}" = "nxp" ]; then
    echo "Using NXP MCU compiler."
    export PREFIX="${1}"
    export PATH="${PATH}:/c/nxp/MCUXpressoIDE_11.4.1_6260/ide/plugins/com.nxp.mcuxpresso.tools.win32_11.4.0.202103011116/tools/bin"
    export CC=arm-none-eabi-gcc
    export AR=arm-none-eabi-ar
    # Extra compiler flags taken from 'Falcon (TCU4G)' project.
    export ECFLAGS="-Og -fno-common -g3 -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fstack-usage -Wstack-usage=4095 -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage"
elif [ "${1}" = "sam" ]; then
    echo "Using Microchip MCU compiler."
    export PREFIX="${1}"
    export PATH="${PATH}:/d/Microchip/xc32/v4.60/bin"
    export CC=xc32-gcc
    export AR=xc32-ar
    # Extra compiler flags taken from 'MAM100' project.
    export ECFLAGS="-g -mprocessor=ATSAME51N20A -ffunction-sections -fdata-sections -O1 -fno-common"
else
    echo "Using default compiler."
    export PREFIX="default"
    export CC=gcc
    export AR=ar
    export ECFLAGS=""
fi
echo "PREFIX is ${PREFIX}"
echo "CC is ${CC}"
echo "AR is ${AR}"
echo


# Step 2: Navigate to internal directory and set module name.
echo "***** Module preparation *****"
export MOD_NAME=asl
echo "Script invoked from: ${PWD}"
echo "Going into internal directory 'Logical View'."
cd Logical\ View
echo "Now inside: ${PWD}"
echo "Target module is '${MOD_NAME}'"
echo


# Step 3: Compile to object files.
echo "***** Compilation *****"
for source_file in ${MOD_NAME}/*.c; do
    export compile_invoke="${CC} -c ${source_file} -I ${MOD_NAME} ${CFLAGS} ${ECFLAGS}"
    echo "${compile_invoke}"
    if ${compile_invoke}; then
        :
    else
        echo "Failure, abort (manual clean-up required)."
        exit
    fi
done
echo


# Step 4: Archive the object files to create static library.
echo "***** Librarize *****"
object_list=""
for object_file in *.o; do
    echo "Adding to object list: ${object_file}"
    object_list="${object_list} ${object_file}"
done
export librarize_invoke="${AR} -rc lib${MOD_NAME}.a ${object_list}"
echo "${librarize_invoke}"
if ${librarize_invoke}; then
    :
else
    echo "Failure, abort (manual clean-up required)."
    exit
fi
echo


# Step 5: Export headers and build tarball.
echo "***** Export *****"
header_list=""
library_file="lib${MOD_NAME}.a"
tar_file="${PREFIX}_lib${MOD_NAME}.tar.gz"
echo "${tar_file}"
for header_file in ${MOD_NAME}/*.h; do
    echo "Adding to header list: ${MOD_NAME}/${header_file}"
    header_list="${header_list} ${header_file}"
done
chmod 444 ${header_list}
export tar_invoke="tar -czvf ${tar_file} ${library_file} ${header_list}"
echo "${tar_invoke}"
if ${tar_invoke}; then
    mkdir -p ../../libs/${MOD_NAME}/
    mv "${tar_file}" ../../libs/${MOD_NAME}/
else
    echo "Failure, abort (manual clean-up required)."
    chmod 644 ${header_list}
    exit
fi
echo


# Step 6: Clean up.
echo "***** Clean-up *****"
rm "${library_file}"
rm *.o
chmod 644 ${header_list}
echo "Graceful finish, have a good day."
exit

