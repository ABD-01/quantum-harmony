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
 *  @version    0.0.1
 *
 *  @date       03-Apr-2025 20:21:02
 *
 *  @brief      Header for the 'asl_cbuf' Class.
*******************************************************************************/

#if !defined(ASL_CBUF_H)
#define ASL_CBUF_H


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
 * Generic 8-bit data circular buffer.
 * This circular buffer is meant to be implemented to be producer-consumer thread-safe only.
 * i.e. only two threads are possible, one of which is producer and another one is consumer.
 */

/**
 * @brief
 * Structure representing circular buffer.
 */
typedef struct asl_cbuf_t
{
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
}  asl_cbuf_t;


/**
 * @brief
 * Circular buffer error codes.
 */
typedef enum asl_cbuf_error_n
{
    /**
     * No error.
     */
    asl_cbuf_error_ok = 0x00000001 << 0,
    /**
     * Parameter is incorrect (mostly for NULL check).
     */
    asl_cbuf_error_param = 0x00000001 << 1,
    /**
     * Usage is incorrect.
     */
    asl_cbuf_error_forbidden = 0x00000001 << 2,
    /**
     * Force 32-bit.
     */
    asl_cbuf_error_max = 0x7FFFFFFF
} asl_cbuf_error_n;

/**
 * @brief
 * Initialize (resets) the circular buffer elements.
 * 
 * @param
 * ptr_cbuf
 * Pointer to circular buffer.
 * 
 * @return
 * asl_cbuf_error_ok           - Success.
 * asl_cbuf_error_param        - NULL pointer or zero sized buffer or other parameter inconsistency.
 */
asl_cbuf_error_n asl_cbuf_init(asl_cbuf_t* ptr_cbuf);

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
 * asl_cbuf_error_ok           - Success.
 * asl_cbuf_error_param        - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_cbuf_error_forbidden    - Not enough free space in circular buffer for requested number of elements. Prevents
 * overflow.
 */
asl_cbuf_error_n asl_cbuf_enqueue(asl_cbuf_t* ptr_cbuf, asl_buffer_t buffer, size_t req_enqueue);

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
 * asl_cbuf_error_ok           - Success.
 * asl_cbuf_error_param        - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_cbuf_error_forbidden    - Requested number of elements not present in circular buffer. Prevents underflow.
 */
asl_cbuf_error_n asl_cbuf_dequeue(asl_cbuf_t* ptr_cbuf, asl_buffer_t buffer, size_t req_dequeue);

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
 * A buffer to which data will be previewed.
 * 
 * @param
 * req_preview
 * The number of elements to be previewed.
 * 
 * @return
 * asl_cbuf_error_ok           - Success.
 * asl_cbuf_error_param        - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_cbuf_error_forbidden    - Requested number of elements not present in circular buffer. Prevents underflow.
 */
asl_cbuf_error_n asl_cbuf_preview(asl_cbuf_t* ptr_cbuf, asl_buffer_t buffer, size_t req_preview);

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
 * asl_cbuf_error_ok           - Success.
 * asl_cbuf_error_param        - NULL pointer or zero sized buffer or other parameter inconsistency.
 */
asl_cbuf_error_n asl_cbuf_available_read(asl_cbuf_t* ptr_cbuf, size_t* available_to_read);

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
 * asl_cbuf_error_ok           - Success.
 * asl_cbuf_error_param        - NULL pointer or zero sized buffer or other parameter inconsistency.
 */
asl_cbuf_error_n asl_cbuf_available_write(asl_cbuf_t* ptr_cbuf, size_t* available_to_write);


#ifdef __cplusplus
}
#endif


#endif /* ASL_CBUF_H */
