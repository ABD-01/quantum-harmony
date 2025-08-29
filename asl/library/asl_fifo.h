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
 *  @version    0.0.2
 *
 *  @date       22 August 2025
 *
 *  @brief      Header for the 'asl_fifo' Class.
 *              A generic circular FIFO for fixed size element size.
 *              This circular FIFO is meant to be implemented to be producer-consumer thread-safe only.
 *              i.e. only two threads are possible, one of which is producer and another one is consumer.
*******************************************************************************/

#if !defined(ASL_FIFO_H)
#define ASL_FIFO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types header. */
#include "asl_fifo_types.h"

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
 * ASL_FIFO_E_OK            - Success.
 * ASL_FIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_FIFO_E_ILLEGAL       - The operation cannot be performed because the FIFO state is corrupt.
 */
asl_fifo_error_e asl_fifo_reset(asl_fifo_s* ptr_fifo);

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
 * ASL_FIFO_E_OK            - Success.
 * ASL_FIFO_E_PARAM         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * ASL_FIFO_E_ILLEGAL       - The operation cannot be performed because the FIFO state is corrupt.
 */
asl_fifo_error_e asl_fifo_get_count_capacity(asl_fifo_s* ptr_fifo, size_t* capacity);

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
 * ASL_FIFO_E_OK            - Success.
 * ASL_FIFO_E_PARAM         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * ASL_FIFO_E_ILLEGAL       - The operation cannot be performed because the FIFO state is corrupt.
 */
asl_fifo_error_e asl_fifo_get_count_used(asl_fifo_s* ptr_fifo, size_t* used);

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
 * ASL_FIFO_E_OK            - Success.
 * ASL_FIFO_E_PARAM         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * ASL_FIFO_E_ILLEGAL       - The operation cannot be performed because the FIFO state is corrupt.
 */
asl_fifo_error_e asl_fifo_get_count_free(asl_fifo_s* ptr_fifo, size_t* free);

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
 * ASL_FIFO_E_OK            - Success.
 * ASL_FIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_FIFO_E_ILLEGAL       - The operation cannot be performed because the FIFO state is corrupt.
 * ASL_FIFO_E_EMPTY         - Operation is not possible since queue is empty.
 */
asl_fifo_error_e asl_fifo_preview(asl_fifo_s* ptr_fifo, asl_buffer_s* element);

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
 * ASL_FIFO_E_OK            - Success.
 * ASL_FIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_FIFO_E_ILLEGAL       - The operation cannot be performed because the FIFO state is corrupt.
 * ASL_FIFO_E_EMPTY         - Operation is not possible since queue is empty.
 */
asl_fifo_error_e asl_fifo_dequeue(asl_fifo_s* ptr_fifo, asl_buffer_s* element);

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
 * ASL_FIFO_E_OK            - Success.
 * ASL_FIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_FIFO_E_ILLEGAL       - The operation cannot be performed because the FIFO state is corrupt.
 * ASL_FIFO_E_FULL          - Operation is not possible since queue is full.
 */
asl_fifo_error_e asl_fifo_enqueue(asl_fifo_s* ptr_fifo, asl_buffer_s element);

#ifdef __cplusplus
}
#endif

#endif /* ASL_FIFO_H */
