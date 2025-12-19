/*******************************************************************************
 *  @file       asl_fifo_priv.h
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
 *  @brief      Private header for the 'asl_fifo' Class.
*******************************************************************************/

#if !defined(ASL_FIFO_PRIV_H)
#define ASL_FIFO_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types includes. */
#include "asl_fifo_types.h"

/**
 * @brief
 * Dequeues the front element of the FIFO.
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
 * ASL_FIFO_E_OK            - Success.
 * ASL_FIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_FIFO_E_ILLEGAL       - The operation cannot be performed because the FIFO state is corrupt.
 * ASL_FIFO_E_EMPTY         - Operation is not possible since queue is empty.
 */
static asl_fifo_error_e asl_fifo__preview_dequeue_private(asl_fifo_s* ptr_fifo, asl_buffer_s* element, bool is_dequeue);

/**
 * @brief
 * Performs a 'circular next' from current index.
 * 
 * @param
 * now
 * The queue index from where to begin.
 * 
 * @param
 * step
 * The number of steps to jump.
 * 
 * @param
 * capacity
 * The number of elements that can be held.
 * 
 * @return
 * The index after performing circular next.
 */
static size_t asl_fifo__circular_next_private(size_t now, size_t step, size_t capacity);

/**
 * @brief
 * Tells number of elements that are already present.
 * 
 * @param
 * index_read
 * Current read pointer (index) of the queue.
 * 
 * @param
 * index_write
 * Current write pointer (index) of the queue.
 * 
 * @param
 * capacity
 * The number of elements that can be held.
 * 
 * @return
 * The number of elements that are already present.
 */
static size_t asl_fifo__available_read_private(size_t index_read, size_t index_write, size_t capacity);

/**
 * @brief
 * Tells max number of elements that can be written.
 * 
 * @param
 * index_read
 * Current read pointer (index) of the queue.
 * 
 * @param
 * index_write
 * Current write pointer (index) of the queue.
 * 
 * @param
 * capacity
 * The number of elements that can be held.
 * 
 * @return
 * Max number of elements that can be written.
 */
static size_t asl_fifo__available_write_private(size_t index_read, size_t index_write, size_t capacity);

#ifdef __cplusplus
}
#endif

#endif /* ASL_FIFO_PRIV_H */
