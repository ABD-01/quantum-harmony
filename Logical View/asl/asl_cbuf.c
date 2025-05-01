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
 *  @date       03-Apr-2025 20:21:02
 *
 *  @brief      Implementation of the 'asl_cbuf' Class.
*******************************************************************************/

#include "asl_cbuf.h"

#define ASL_CBUF_ENABLE_PARAM_CHECK ( 1 )

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
 * The capacity of backing buffer (i.e. size of backing buffer).
 * 
 * @return
 * The index after performing circular next.
 */
static size_t circular_next(size_t now, size_t step, size_t capacity);

/**
 * @brief
 * Tells number of elements that are already present.
 * 
 * @param
 * front
 * Current front (index) of the queue.
 * 
 * @param
 * rear
 * Current rear (index) of the queue.
 * 
 * @param
 * capacity
 * The capacity of backing buffer (i.e. size of backing buffer).
 * 
 * @return
 * The number of elements that are already present.
 */
static size_t available_read(size_t front, size_t rear, size_t capacity);

/**
 * @brief
 * Tells max number of elements that can be written.
 * 
 * @param
 * front
 * Current front (index) of the queue.
 * 
 * @param
 * rear
 * Current rear (index) of the queue.
 * 
 * @param
 * capacity
 * The capacity of backing buffer (i.e. size of backing buffer).
 * 
 * @return
 * Max number of elements that can be written.
 */
static size_t available_write(size_t front, size_t rear, size_t capacity);



asl_cbuf_error_n asl_cbuf_init(asl_cbuf_t* ptr_cbuf)
{
    asl_cbuf_error_n err = asl_cbuf_error_param;
    size_t i = 0;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem ) {
    #endif
        ptr_cbuf->front = 0;
        ptr_cbuf->rear = 0;
        for ( i = 0 ; i < ptr_cbuf->size_mem ; ++i ) {
            ptr_cbuf->mem[i] = 0;
        }
        err = asl_cbuf_error_ok;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    }
    #endif
    return err;
}

asl_cbuf_error_n asl_cbuf_enqueue(asl_cbuf_t* ptr_cbuf, asl_buffer_t buffer, size_t req_enqueue)
{
    asl_cbuf_error_n err = asl_cbuf_error_param;
    size_t i = 0;
    size_t rear = 0;
    size_t capacity  = 0;
    size_t available_to_write = 0;
    size_t new_rear = 0;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem && ( ptr_cbuf->front < ptr_cbuf->size_mem ) && ( ptr_cbuf->rear < ptr_cbuf->size_mem ) ) {
        if ( buffer.ptr && buffer.size && req_enqueue && ( req_enqueue < ptr_cbuf->size_mem ) && ( req_enqueue <= buffer.size ) ) {
    #endif
            rear = ptr_cbuf->rear;
            capacity = ptr_cbuf->size_mem;
            available_to_write = available_write(ptr_cbuf->front, rear, capacity);
            if ( req_enqueue <= available_to_write ) {
                new_rear = circular_next(rear, req_enqueue, capacity);
                do {
                    ptr_cbuf->mem[rear] = buffer.ptr[i++];
                    rear = circular_next(rear, 1, capacity);
                } while ( rear != new_rear );
                ptr_cbuf->rear = rear;
                err = asl_cbuf_error_ok;
            }
            else {
                err = asl_cbuf_error_forbidden;
            }
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
        }
    }
    #endif
    return err;
}

asl_cbuf_error_n asl_cbuf_dequeue(asl_cbuf_t* ptr_cbuf, asl_buffer_t buffer, size_t req_dequeue)
{
    asl_cbuf_error_n err = asl_cbuf_error_param;
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
            available_to_read = available_read(front, ptr_cbuf->rear, capacity);
            if ( req_dequeue <= available_to_read ) {
                new_front = circular_next(front, req_dequeue, capacity);
                do {
                    buffer.ptr[i++] = ptr_cbuf->mem[front] ;
                    front = circular_next(front, 1, capacity);
                } while ( front != new_front );
                ptr_cbuf->front = front;
                err = asl_cbuf_error_ok;
            }
            else {
                err = asl_cbuf_error_forbidden;
            }
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
        }
    }
    #endif
    return err;
}

asl_cbuf_error_n asl_cbuf_preview(asl_cbuf_t* ptr_cbuf, asl_buffer_t buffer, size_t req_preview)
{
    asl_cbuf_error_n err = asl_cbuf_error_param;
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
            available_to_read = available_read(front, ptr_cbuf->rear, capacity);
            if ( req_preview <= available_to_read ) {
                new_front = circular_next(front, req_preview, capacity);
                do {
                    buffer.ptr[i++] = ptr_cbuf->mem[front] ;
                    front = circular_next(front, 1, capacity);
                } while ( front != new_front );
                // We don't update the actual CBUF front since this is just a preview.
                err = asl_cbuf_error_ok;
            }
            else {
                err = asl_cbuf_error_forbidden;
            }
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
        }
    }
    #endif
    return err;
}

asl_cbuf_error_n asl_cbuf_available_read(asl_cbuf_t* ptr_cbuf, size_t* available_to_read)
{
    asl_cbuf_error_n err = asl_cbuf_error_param;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem && ( ptr_cbuf->front < ptr_cbuf->size_mem ) && ( ptr_cbuf->rear < ptr_cbuf->size_mem ) && available_to_read ) {
    #endif
        *available_to_read = available_read(ptr_cbuf->front, ptr_cbuf->rear, ptr_cbuf->size_mem);
        err = asl_cbuf_error_ok;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    }
    #endif
    return err;
}

asl_cbuf_error_n asl_cbuf_available_write(asl_cbuf_t* ptr_cbuf, size_t* available_to_write)
{
    asl_cbuf_error_n err = asl_cbuf_error_param;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    if ( ptr_cbuf && ptr_cbuf->mem && ptr_cbuf->size_mem && ( ptr_cbuf->front < ptr_cbuf->size_mem ) && ( ptr_cbuf->rear < ptr_cbuf->size_mem ) && available_to_write ) {
    #endif
        *available_to_write = available_write(ptr_cbuf->front, ptr_cbuf->rear, ptr_cbuf->size_mem);
        err = asl_cbuf_error_ok;
    #if ASL_CBUF_ENABLE_PARAM_CHECK == 1
    }
    #endif
    return err;
}

static size_t circular_next(size_t now, size_t step, size_t capacity)
{
    return ( ( now + step ) % capacity );
}

static size_t available_read(size_t front, size_t rear, size_t capacity)
{
    return ( capacity - 1 - ( ( capacity - rear + front - 1 ) % capacity ) );
}

static size_t available_write(size_t front, size_t rear, size_t capacity)
{
	return ( capacity - rear + front - 1 ) % capacity;
} 

