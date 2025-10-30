/*******************************************************************************
 *  @file       asl_cbuf_types.h
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
 *  @date       27 October 2025
 *
 *  @brief      Types header for the 'asl_cbuf' Class.
*******************************************************************************/

#if !defined(ASL_CBUF_TYPES_H)
#define ASL_CBUF_TYPES_H

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

/**
 * @brief
 * Structure representing circular buffer.
 */
typedef struct asl_cbuf_s {
    /**
     * Pointer to circular buffer backing memory.
     */
    puint8_t mem;
    /**
     * Size of circular buffer backing memory.
     */
    size_t size_mem;
    /**
     * Front element index of circular buffer presently.
     * NOTE: This is sometimes referred to as 'read_pointer', it is the read index.
     */
    size_t front;
    /**
     * Rear element index of circular buffer presently.
     * NOTE: This is sometimes referred to as 'write_pointer', it is the write index.
     */
    size_t rear;
} asl_cbuf_s;

/**
 * @brief
 * Circular buffer error codes.
 */
typedef enum asl_cbuf_error_e {
    /**
     * No error.
     */
    ASL_CBUF_E_OK = 0x00000001 << 0,
    /**
     * Parameter is incorrect (mostly for NULL check).
     */
    ASL_CBUF_E_PARAM = 0x00000001 << 1,
    /**
     * Usage is incorrect.
     */
    ASL_CBUF_E_FORBID = 0x00000001 << 2,
    /**
     * Force 32-bit.
     */
    ASL_CBUF_E_MAX = 0x7FFFFFFF
} asl_cbuf_error_e;

#ifdef __cplusplus
}
#endif

#endif /* ASL_CBUF_TYPES_H */
