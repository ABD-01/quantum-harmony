/*****************************************************************************
File:           linker.ld
Author:         Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
Date Created:   15 April 2025
Description:    Linker script to place the .boot_info section at a fixed address 
                for bootloader metadata, and suppress unused sections like .text.

Copyright (c) 2024-2025 Accolade Electronics Pvt. Ltd. All Rights Reserved.

Changelog:

  2025-04-15  Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
    - Created linker script for boot_info section.

References:
  - https://software-dl.ti.com/ccs/esd/documents/sdto_cgt_Linker-Command-File-Primer.html#nullify-an-output-section

*****************************************************************************/

MEMORY
{
    DUMMY           (RX)  : origin = 0x00000000, length = 0x00009000
    BOOT_INFO_1     (RX)  : origin = 0x00009000, length = 0x00000400
}

SECTIONS
{
    .boot_info : 
    {
        *(.boot_info)
    } > BOOT_INFO_1

    .text : > DUMMY, TYPE = DSECT    /* not used */
}
