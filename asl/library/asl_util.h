/*******************************************************************************
 *  @file       asl_util.h
 *
 *  @copyright  Accolade Electronics Pvt Ltd, 2025-26
 *              All Rights Reserved
 *              UNPUBLISHED, LICENSED SOFTWARE.
 *              Accolade Electronics, Pune
 *              CONFIDENTIAL AND PROPRIETARY INFORMATION
 *              WHICH IS THE PROPERTY OF M/s Accolade Electronics.
 *
 *  @version    0.0.2
 *
 *  @date       22 August 2025
 *
 *  @brief      Header for the 'asl_util' Class.
*******************************************************************************/

#if !defined(ASL_UTIL_H)
#define ASL_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types header. */
#include "asl_util_types.h"

/**
 * @brief
 * Utilities related to ASL internally.
 * They can also be used by end-users.
 */

/**
 * @brief
 * Set a buffer with fill value.
 * 
 * @param
 * dest
 * Destination buffer.
 * 
 * @param
 * value
 * Fill value.
 */
void asl_util_buffer_memset(asl_buffer_s dest, uint8_t value);

/**
 * @brief
 * Copy buffer from source to destination.
 * 
 * @param
 * dest
 * Destination buffer.
 * 
 * @param
 * src
 * Source buffer.
 */
void asl_util_buffer_memcpy(asl_buffer_s dest, asl_buffer_s src);

#ifdef __cplusplus
}
#endif

#endif /* ASL_UTIL_H */
