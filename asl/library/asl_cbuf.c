/*******************************************************************************
 *  @file       asl_cbuf.c
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
 *  @brief      Implementation of the 'asl_cbuf' Class.
*******************************************************************************/

/* Self. */
#include "asl_cbuf.h"
#include "asl_cbuf_priv.h"

asl_cbuf_error_e asl_cbuf__init(asl_cbuf_s* ptr_cbuf) {
    asl_cbuf_error_e error = ASL_CBUF_E_PARAM;
    size_t i = 0;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem ) {
    #endif
        ptr_cbuf->front = 0;
        ptr_cbuf->rear = 0;
        for ( i = 0 ; i < ptr_cbuf->size_mem ; ++i ) {
            ptr_cbuf->mem[i] = 0;
        }
        error = ASL_CBUF_E_OK;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    }
    #endif
    return error;
}

asl_cbuf_error_e asl_cbuf__enqueue(asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_enqueue) {
    asl_cbuf_error_e error = ASL_CBUF_E_PARAM;
    size_t i = 0;
    size_t rear = 0;
    size_t capacity = 0;
    size_t available_to_write = 0;
    size_t new_rear = 0;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem && ( ptr_cbuf->front < ptr_cbuf->size_mem ) && ( ptr_cbuf->rear < ptr_cbuf->size_mem ) ) {
        if ( buffer.ptr && buffer.size && req_enqueue && ( req_enqueue < ptr_cbuf->size_mem ) && ( req_enqueue <= buffer.size ) ) {
    #endif
            rear = ptr_cbuf->rear;
            capacity = ptr_cbuf->size_mem;
            available_to_write = asl_cbuf__available_write_private(ptr_cbuf->front, rear, capacity);
            if ( req_enqueue <= available_to_write ) {
                new_rear = asl_cbuf__circular_next_private(rear, req_enqueue, capacity);
                do {
                    ptr_cbuf->mem[rear] = buffer.ptr[i++];
                    rear = asl_cbuf__circular_next_private(rear, 1, capacity);
                } while ( rear != new_rear );
                ptr_cbuf->rear = rear;
                error = ASL_CBUF_E_OK;
            } else {
                error = ASL_CBUF_E_FORBID;
            }
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
        }
    }
    #endif
    return error;
}

asl_cbuf_error_e asl_cbuf__dequeue(asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_dequeue) {
    asl_cbuf_error_e error = ASL_CBUF_E_PARAM;
    size_t i = 0;
    size_t front = 0;
    size_t capacity = 0;
    size_t available_to_read = 0;
    size_t new_front = 0;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem && ( ptr_cbuf->front < ptr_cbuf->size_mem ) && ( ptr_cbuf->rear < ptr_cbuf->size_mem ) ) {
        if ( buffer.ptr && buffer.size && req_dequeue && ( req_dequeue < ptr_cbuf->size_mem ) && ( req_dequeue <= buffer.size ) ) {
    #endif
            front = ptr_cbuf->front;
            capacity = ptr_cbuf->size_mem;
            available_to_read = asl_cbuf__available_read_private(front, ptr_cbuf->rear, capacity);
            if ( req_dequeue <= available_to_read ) {
                new_front = asl_cbuf__circular_next_private(front, req_dequeue, capacity);
                do {
                    buffer.ptr[i++] = ptr_cbuf->mem[front];
                    front = asl_cbuf__circular_next_private(front, 1, capacity);
                } while ( front != new_front );
                ptr_cbuf->front = front;
                error = ASL_CBUF_E_OK;
            } else {
                error = ASL_CBUF_E_FORBID;
            }
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
        }
    }
    #endif
    return error;
}

asl_cbuf_error_e asl_cbuf__preview(asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_preview) {
    asl_cbuf_error_e error = ASL_CBUF_E_PARAM;
    size_t i = 0;
    size_t front = 0;
    size_t capacity = 0;
    size_t available_to_read = 0;
    size_t new_front = 0;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem && ( ptr_cbuf->front < ptr_cbuf->size_mem ) && ( ptr_cbuf->rear < ptr_cbuf->size_mem ) ) {
        if ( buffer.ptr && buffer.size && req_preview && ( req_preview < ptr_cbuf->size_mem ) && ( req_preview <= buffer.size ) ) {
    #endif
            front = ptr_cbuf->front;
            capacity = ptr_cbuf->size_mem;
            available_to_read = asl_cbuf__available_read_private(front, ptr_cbuf->rear, capacity);
            if ( req_preview <= available_to_read ) {
                new_front = asl_cbuf__circular_next_private(front, req_preview, capacity);
                do {
                    buffer.ptr[i++] = ptr_cbuf->mem[front];
                    front = asl_cbuf__circular_next_private(front, 1, capacity);
                } while ( front != new_front );
                /* We don't update the actual CBUF front since this is just a preview. */
                error = ASL_CBUF_E_OK;
            } else {
                error = ASL_CBUF_E_FORBID;
            }
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
        }
    }
    #endif
    return error;
}

asl_cbuf_error_e asl_cbuf__available_read(asl_cbuf_s* ptr_cbuf, size_t* available_to_read) {
    asl_cbuf_error_e error = ASL_CBUF_E_PARAM;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem && ( ptr_cbuf->front < ptr_cbuf->size_mem ) && ( ptr_cbuf->rear < ptr_cbuf->size_mem ) && available_to_read ) {
    #endif
        *available_to_read = asl_cbuf__available_read_private(ptr_cbuf->front, ptr_cbuf->rear, ptr_cbuf->size_mem);
        error = ASL_CBUF_E_OK;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    }
    #endif
    return error;
}

asl_cbuf_error_e asl_cbuf__available_write(asl_cbuf_s* ptr_cbuf, size_t* available_to_write) {
    asl_cbuf_error_e error = ASL_CBUF_E_PARAM;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem && ( ptr_cbuf->front < ptr_cbuf->size_mem ) && ( ptr_cbuf->rear < ptr_cbuf->size_mem ) && available_to_write ) {
    #endif
        *available_to_write = asl_cbuf__available_write_private(ptr_cbuf->front, ptr_cbuf->rear, ptr_cbuf->size_mem);
        error = ASL_CBUF_E_OK;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    }
    #endif
    return error;
}

static size_t asl_cbuf__circular_next_private(size_t now, size_t step, size_t capacity) {
    return ( ( now + step ) % capacity );
}

static size_t asl_cbuf__available_read_private(size_t front, size_t rear, size_t capacity) {
    return ( capacity - 1 - ( ( capacity - rear + front - 1 ) % capacity ) );
}

static size_t asl_cbuf__available_write_private(size_t front, size_t rear, size_t capacity) {
    return ( capacity - rear + front - 1 ) % capacity;
}
