/*******************************************************************************
 *  @file       asl_util.c
 *
 *  @copyright  Accolade Electronics Pvt Ltd, 2025-26
 *              All Rights Reserved
 *              UNPUBLISHED, LICENSED SOFTWARE.
 *              Accolade Electronics, Pune
 *              CONFIDENTIAL AND PROPRIETARY INFORMATION
 *              WHICH IS THE PROPERTY OF M/s Accolade Electronics.
 *
 *  @version    0.0.1
 *
 *  @date       27 November 2025
 *
 *  @brief      Implementation of the 'asl_util' Class.
*******************************************************************************/

/* Self. */
#include "asl_util.h"
#include "asl_util_priv.h"

void asl_util__buffer_memset(asl_buffer_s dest, uint8_t value) {
    if ( dest.ptr ) {
        for ( size_t i = 0 ; i < dest.size ; ++i ) {
            dest.ptr[i] = value;
        }
    }
}

void asl_util__buffer_memcpy(asl_buffer_s dest, asl_buffer_s src) {
    const size_t copy_size = dest.size < src.size ? dest.size : src.size;
    if ( dest.ptr && src.ptr ) {
        for ( size_t i = 0 ; i < copy_size ; ++i ) {
            dest.ptr[i] = src.ptr[i];
        }
    }
}
