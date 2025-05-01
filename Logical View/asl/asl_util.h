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
 *  @version    0.0.1
 *
 *  @date       03-Apr-2025 20:21:03
 *
 *  @brief      Header for the 'asl_util' Class.
*******************************************************************************/

#if !defined(ASL_UTIL_H)
#define ASL_UTIL_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "asl_types.h"
#include "asl_pointer_types.h"

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
void asl_util_buffer_memset(asl_buffer_t dest, uint8_t value);

/**
 * @brief
 * Set a buffer with fill value.
 * 
 * @param
 * dest
 * Destination buffer.
 * 
 * @param
 * src
 * Source buffer.
 */
void asl_util_buffer_memcpy(asl_buffer_t dest, asl_buffer_t src);


#ifdef __cplusplus
}
#endif


#endif /* ASL_UTIL_H */
