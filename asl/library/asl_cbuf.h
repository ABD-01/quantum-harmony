/*******************************************************************************
 *  @file       asl_cbuf.h
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
 *  @brief      Header for the 'asl_cbuf' Class.
 *              Generic 8-bit data circular buffer.
 *              This circular buffer is meant to be implemented to be producer-consumer thread-safe only.
 *              i.e. only two threads are possible, one of which is producer and another one is consumer.
*******************************************************************************/

#if !defined(ASL_CBUF_H)
#define ASL_CBUF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types header. */
#include "asl_cbuf_types.h"

/**
 * @brief
 * Initialize (resets) the circular buffer elements.
 * 
 * @param
 * ptr_cbuf
 * Pointer to circular buffer.
 * 
 * @return
 * ASL_CBUF_E_OK            - Success.
 * ASL_CBUF_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 */
asl_cbuf_error_e asl_cbuf_init(asl_cbuf_s* ptr_cbuf);

/**
 * @brief
 * Enqueues the requested number of elements into circular buffer.
 * 
 * @param
 * ptr_cbuf
 * Pointer to circular buffer.
 * 
 * @param
 * buffer
 * A buffer containing data to be enqueued.
 * 
 * @param
 * req_enqueue
 * The number of elements to be enqueued.
 * 
 * @return
 * ASL_CBUF_E_OK            - Success.
 * ASL_CBUF_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_CBUF_E_FORBID        - Not enough free space in circular buffer for requested number of elements. Prevents overflow.
 */
asl_cbuf_error_e asl_cbuf_enqueue(asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_enqueue);

/**
 * @brief
 * Dequeues the requested number of elements from circular buffer.
 * 
 * @param
 * ptr_cbuf
 * Pointer to circular buffer.
 * 
 * @param
 * buffer
 * A buffer to which data will be dequeued.
 * 
 * @param
 * req_dequeue
 * The number of elements to be dequeued.
 * 
 * @return
 * ASL_CBUF_E_OK            - Success.
 * ASL_CBUF_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_CBUF_E_FORBID        - Requested number of elements not present in circular buffer. Prevents underflow.
 */
asl_cbuf_error_e asl_cbuf_dequeue(asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_dequeue);

/**
 * @brief
 * Previews the requested number of elements from circular buffer.
 * 
 * @param
 * ptr_cbuf
 * Pointer to circular buffer.
 * 
 * @param
 * buffer
 * A buffer to which data will be previewed.
 * 
 * @param
 * req_preview
 * The number of elements to be previewed.
 * 
 * @return
 * ASL_CBUF_E_OK            - Success.
 * ASL_CBUF_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_CBUF_E_FORBID        - Requested number of elements not present in circular buffer. Prevents underflow.
 */
asl_cbuf_error_e asl_cbuf_preview(asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_preview);

/**
 * @brief
 * Tells how many elements are present and thus can be read in circular buffer.
 * 
 * @param
 * ptr_cbuf
 * Pointer to circular buffer.
 * 
 * @param
 * available_to_read
 * When parameters are correct, the number of elements possible to read will be updated into this pointer.
 * 
 * @return
 * ASL_CBUF_E_OK            - Success.
 * ASL_CBUF_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 */
asl_cbuf_error_e asl_cbuf_available_read(asl_cbuf_s* ptr_cbuf, size_t* available_to_read);

/**
 * @brief
 * Tells how many elements can yet be added into the circular buffer.
 * 
 * @param
 * ptr_cbuf
 * Pointer to circular buffer.
 * 
 * @param
 * available_to_write
 * When parameters are correct, the number of elements possible to write will be updated into this pointer.
 * 
 * @return
 * ASL_CBUF_E_OK            - Success.
 * ASL_CBUF_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 */
asl_cbuf_error_e asl_cbuf_available_write(asl_cbuf_s* ptr_cbuf, size_t* available_to_write);

#ifdef __cplusplus
}
#endif

#endif /* ASL_CBUF_H */
