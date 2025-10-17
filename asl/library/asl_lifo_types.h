/*******************************************************************************
 *  @file       asl_lifo_types.h
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
 *  @date       17 October 2025
 *
 *  @brief      Types header for the 'asl_lifo' Class.
*******************************************************************************/

#if !defined(ASL_LIFO_TYPES_H)
#define ASL_LIFO_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ASL includes. */
#include "asl_types.h"
#include "asl_pointer_types.h"
#include "asl_util_types.h"

/**
 * @brief
 * LIFO context definition.
 * Number of elements is inferred from elements.size / size_element and cannot evaluate to zero.
 */
typedef struct asl_lifo_s {
    /**
     * Size in bytes of a single element in the LIFO.
     */
    size_t size_element;
    /**
     * Buffer that stores actual elements of LIFO.
     */
    asl_buffer_s elements;
    /**
     * Zero base relative 'top' of stack.
     * When [ 'top' == 0 ] - it means stack is empty (no elements).
     * When [ 'top' == elements.size/size_element ] - it means stack is full.
     */
    size_t top;
} asl_lifo_s;

/**
 * @brief
 * LIFO API error codes.
 */
typedef enum asl_lifo_error_e {
    /**
     * No error.
     */
    ASL_LIFO_E_OK = 0x00000001 << 0,
    /**
     * Parameter is incorrect (mostly for NULL check).
     */
    ASL_LIFO_E_PARAM = 0x00000001 << 1,
    /**
     * The LIFO context is in illegal state.
     */
    ASL_LIFO_E_ILLEGAL = 0x00000001 << 2,
    /**
     * The LIFO pop/peek is not possible since LIFO is empty.
     */
    ASL_LIFO_E_EMPTY = 0x00000001 << 3,
    /**
     * The LIFO push is not possible since LIFO is full.
     */
    ASL_LIFO_E_FULL = 0x00000001 << 4,
    /**
     * Force 32-bit.
     */
    ASL_LIFO_E_MAX = 0x7FFFFFFF
} asl_lifo_error_e;

#ifdef __cplusplus
}
#endif

#endif /* ASL_LIFO_TYPES_H */
