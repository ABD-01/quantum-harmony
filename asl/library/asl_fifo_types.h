/*******************************************************************************
 *  @file       asl_fifo_types.h
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
 *  @brief      Types header for the 'asl_fifo' Class.
*******************************************************************************/

#if !defined(ASL_FIFO_TYPES_H)
#define ASL_FIFO_TYPES_H

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
 * FIFO API error codes.
 */
typedef enum asl_fifo_error_e
{
    /**
     * No error.
     */
    ASL_FIFO_E_OK = 0x00000001 << 0,
    /**
     * Parameter is incorrect (mostly for NULL check).
     */
    ASL_FIFO_E_PARAM = 0x00000001 << 1,
    /**
     * The FIFO context is in illegal state.
     */
    ASL_FIFO_E_ILLEGAL = 0x00000001 << 2,
    /**
     * The FIFO dequeue/preview is not possible since FIFO is empty.
     */
    ASL_FIFO_E_EMPTY = 0x00000001 << 3,
    /**
     * The FIFO enqueue is not possible since FIFO is full.
     */
    ASL_FIFO_E_FULL = 0x00000001 << 4,
    /**
     * Force 32-bit.
     */
    ASL_FIFO_E_MAX = 0x7FFFFFFF
} asl_fifo_error_e;

/**
 * @brief
 * FIFO context definition.
 * Number of elements is inferred from cbuf.size_mem / size_element and cannot evaluate to zero.
 */
typedef struct asl_fifo_s
{
    /**
     * Buffer that stores actual elements of FIFO.
     */
    asl_buffer_s elements;
    /**
     * Size in bytes of a single element in the FIFO.
     */
    size_t size_element;
    /**
     * Write index of circular queue - used by producer during enqueue.
     */
    size_t index_write;
    /**
     * Read index of circular queue - used by consumer during preview/dequeue.
     */
    size_t index_read;
} asl_fifo_s;

#ifdef __cplusplus
}
#endif

#endif /* ASL_FIFO_TYPES_H */
