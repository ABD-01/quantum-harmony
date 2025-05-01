/*******************************************************************************
 *  @file       asl_fifo.h
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
 *  @date       03-Apr-2025 20:21:02
 *
 *  @brief      Header for the 'asl_fifo' Class.
*******************************************************************************/

#if !defined(ASL_FIFO_H)
#define ASL_FIFO_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "asl_types.h"
#include "asl_util.h"
#include "asl_pointer_types.h"

/**
 * @brief
 * A generic circular FIFO for fixed size element size.
 * This circular FIFO is meant to be implemented to be producer-consumer thread-safe only.
 * i.e. only two threads are possible, one of which is producer and another one is consumer.
 */

/**
 * @brief
 * FIFO API error codes.
 */
typedef enum asl_fifo_error_n
{
    /**
     * No error.
     */
    asl_fifo_error_ok = 0x00000001 << 0,
    /**
     * Parameter is incorrect (mostly for NULL check).
     */
    asl_fifo_error_param = 0x00000001 << 1,
    /**
     * The FIFO context is in illegal state.
     */
    asl_fifo_error_illegal_state = 0x00000001 << 2,
    /**
     * The FIFO dequeue/preview is not possible since FIFO is empty.
     */
    asl_fifo_error_empty = 0x00000001 << 3,
    /**
     * The FIFO enqueue is not possible since FIFO is full.
     */
    asl_fifo_error_full = 0x00000001 << 4,
    /**
     * Force 32-bit.
     */
    asl_fifo_error_max = 0x7FFFFFFF
} asl_fifo_error_n;

/**
 * @brief
 * FIFO context definition.
 * Number of elements is inferred from cbuf.size_mem / size_element and cannot evaluate to zero.
 */
typedef struct asl_fifo_t
{
    /**
     * Buffer that stores actual elements of FIFO.
     */
    asl_buffer_t elements;
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
    int index_read;
}  asl_fifo_t;

/**
 * @brief
 * Resets the FIFO context.
 * It is recommended (but not strictly required) to do FIFO reset before first use.
 * 
 * @param
 * ptr_fifo
 * Pointer to FIFO context.
 * 
 * @return
 * asl_fifo_error_ok            - Success.
 * asl_fifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_fifo_error_illegal_state - The operation cannot be performed because the FIFO state is corrupt.
 */
asl_fifo_error_n asl_fifo_reset(asl_fifo_t* ptr_fifo);

/**
 * @brief
 * Gets the capacity (maximum number of elements) possible in the FIFO.
 * 
 * @param
 * ptr_fifo
 * Pointer to FIFO context.
 * 
 * @param
 * capacity
 * The maximum number of elements possible in the FIFO.
 * 
 * @return
 * asl_fifo_error_ok            - Success.
 * asl_fifo_error_param         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * asl_fifo_error_illegal_state - The operation cannot be performed because the FIFO state is corrupt.
 */
asl_fifo_error_n asl_fifo_get_count_capacity(asl_fifo_t* ptr_fifo, size_t* capacity);

/**
 * @brief
 * Gets the used number of element slots in the FIFO.
 * 
 * @param
 * ptr_fifo
 * Pointer to FIFO context.
 * 
 * @param
 * used
 * The used number of element slots in the FIFO.
 * 
 * @return
 * asl_fifo_error_ok            - Success.
 * asl_fifo_error_param         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * asl_fifo_error_illegal_state - The operation cannot be performed because the FIFO state is corrupt.
 */
asl_fifo_error_n asl_fifo_get_count_used(asl_fifo_t* ptr_fifo, size_t* used);

/**
 * @brief
 * Gets the free number of element slots in the FIFO.
 * 
 * @param
 * ptr_fifo
 * Pointer to FIFO context.
 * 
 * @param
 * free
 * The free number of element slots in the FIFO.
 * 
 * @return
 * asl_fifo_error_ok            - Success.
 * asl_fifo_error_param         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * asl_fifo_error_illegal_state - The operation cannot be performed because the FIFO state is corrupt.
 */
asl_fifo_error_n asl_fifo_get_count_free(asl_fifo_t* ptr_fifo, size_t* free);

/**
 * @brief
 * Previews front element of the FIFO.
 * This operation does not dequeue the element.
 * 
 * @param
 * ptr_fifo
 * Pointer to FIFO context.
 * 
 * @param
 * element
 * The element that is at front, if queue is not empty.
 * 
 * @return
 * asl_fifo_error_ok            - Success.
 * asl_fifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_fifo_error_illegal_state - The operation cannot be performed because the FIFO state is corrupt.
 * asl_fifo_error_empty         - Operation is not possible since queue is empty.
 */
asl_fifo_error_n asl_fifo_preview(asl_fifo_t* ptr_fifo, asl_buffer_t* element);

/**
 * @brief
 * Previews or Dequeues the front element of the FIFO.
 * 
 * @param
 * ptr_fifo
 * Pointer to FIFO context.
 * 
 * @param
 * element
 * The element that is at front, if queue is not empty.
 * 
 * @param
 * is_dequeue
 * Whether to actually Dequeue or just Preview.
 * 
 * @return
 * asl_fifo_error_ok            - Success.
 * asl_fifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_fifo_error_illegal_state - The operation cannot be performed because the FIFO state is corrupt.
 * asl_fifo_error_empty         - Operation is not possible since queue is empty.
 */
asl_fifo_error_n asl_fifo_dequeue(asl_fifo_t* ptr_fifo, asl_buffer_t* element);

/**
 * @brief
 * Enqueues an element into the FIFO.
 * 
 * @param
 * ptr_fifo
 * Pointer to FIFO context.
 * 
 * @param
 * element
 * The element that needs to be enqueued.
 * 
 * @return
 * asl_fifo_error_ok            - Success.
 * asl_fifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_fifo_error_illegal_state - The operation cannot be performed because the FIFO state is corrupt.
 * asl_fifo_error_full          - Operation is not possible since queue is full.
 */
asl_fifo_error_n asl_fifo_enqueue(asl_fifo_t* ptr_fifo, asl_buffer_t element);


#ifdef __cplusplus
}
#endif


#endif /* ASL_FIFO_H */
