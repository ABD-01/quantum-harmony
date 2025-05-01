 /*******************************************************************************
 *  @file       asl_fifo.c
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
 *  @brief      Implementation of the 'asl_fifo' Class.
*******************************************************************************/

#include "asl_fifo.h"

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
 * asl_fifo_error_ok            - Success.
 * asl_fifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_fifo_error_illegal_state - The operation cannot be performed because the FIFO state is corrupt.
 * asl_fifo_error_empty         - Operation is not possible since queue is empty.
 */
static asl_fifo_error_n preview_dequeue(asl_fifo_t* ptr_fifo, asl_buffer_t* element, bool is_dequeue);

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
static size_t circular_next(size_t now, size_t step, size_t capacity);

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
static size_t available_read(size_t index_read, size_t index_write, size_t capacity);

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
static size_t available_write(size_t index_read, size_t index_write, size_t capacity);



asl_fifo_error_n asl_fifo_reset(asl_fifo_t* ptr_fifo)
{
    asl_fifo_error_n error = asl_fifo_error_max;
    asl_fifo_t* const fifo = ptr_fifo;
    asl_buffer_t paint;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element ) {
        /* Found NULL or zero size in parameter. */
        error = asl_fifo_error_param;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = asl_fifo_error_illegal_state;
        } else if (  1 == k_count_max_elements  ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = asl_fifo_error_illegal_state;
        } else {
            /* Success - set the top to zero and paint the element slots and remainder area for debugging. */
            fifo->index_read = 0;
            fifo->index_write = 0;
            for ( size_t i = 0; i < k_count_max_elements ; ++i ) {
                paint.ptr = fifo->elements.ptr + ( i * k_size_element );
                paint.size = k_size_element;
                asl_util_buffer_memset(paint, (uint8_t) i);
            }
            paint.ptr = fifo->elements.ptr + ( k_count_max_elements * k_size_element );
            paint.size = fifo->elements.size - ( k_count_max_elements * k_size_element );
            asl_util_buffer_memset(paint, (uint8_t) 0xFF);
            error = asl_fifo_error_ok;
        }
    }
    return error;
}

asl_fifo_error_n asl_fifo_get_count_capacity(asl_fifo_t* ptr_fifo, size_t* capacity)
{
    asl_fifo_error_n error = asl_fifo_error_max;
    asl_fifo_t* const fifo = ptr_fifo;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !capacity ) {
        /* Found NULL or zero size in parameter. */
        error = asl_fifo_error_param;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = asl_fifo_error_illegal_state;
        } if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = asl_fifo_error_illegal_state;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else {
            /* Success - tell capacity count. */
            *capacity = k_count_max_elements - 1;
            error = asl_fifo_error_ok;
        }
    }
    return error;
}

asl_fifo_error_n asl_fifo_get_count_used(asl_fifo_t* ptr_fifo, size_t* used)
{
    asl_fifo_error_n error = asl_fifo_error_max;
    asl_fifo_t* const fifo = ptr_fifo;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !used ) {
        /* Found NULL or zero size in parameter. */
        error = asl_fifo_error_param;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = asl_fifo_error_illegal_state;
        } if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = asl_fifo_error_illegal_state;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else {
            /* Success - tell used count. */
            const size_t index_read = fifo->index_read;
            const size_t index_write = fifo->index_write;
            *used = available_read(index_read, index_write, k_count_max_elements);
            error = asl_fifo_error_ok;
        }
    }
    return error;
}

asl_fifo_error_n asl_fifo_get_count_free(asl_fifo_t* ptr_fifo, size_t* free)
{
    asl_fifo_error_n error = asl_fifo_error_max;
    asl_fifo_t* const fifo = ptr_fifo;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !free ) {
        /* Found NULL or zero size in parameter. */
        error = asl_fifo_error_param;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = asl_fifo_error_illegal_state;
        } if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = asl_fifo_error_illegal_state;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else {
            /* Success - tell free count. */
            const size_t index_read = fifo->index_read;
            const size_t index_write = fifo->index_write;
            *free = available_write(index_read, index_write, k_count_max_elements);
            error = asl_fifo_error_ok;
        }
    }
    return error;
}

asl_fifo_error_n asl_fifo_preview(asl_fifo_t* ptr_fifo, asl_buffer_t* element)
{
    return preview_dequeue(ptr_fifo, element, false);
}

asl_fifo_error_n asl_fifo_dequeue(asl_fifo_t* ptr_fifo, asl_buffer_t* element)
{
    return preview_dequeue(ptr_fifo, element, true);
}

asl_fifo_error_n asl_fifo_enqueue(asl_fifo_t* ptr_fifo, asl_buffer_t element)
{
    asl_fifo_error_n error = asl_fifo_error_max;
    asl_fifo_t* const fifo = ptr_fifo;
    asl_buffer_t dest;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !element.ptr || !element.size ) {
        /* Found NULL or zero size in parameter. */
        error = asl_fifo_error_param;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = asl_fifo_error_illegal_state;
        } if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = asl_fifo_error_illegal_state;
        } else if ( k_size_element != element.size ) {
            /* Found mismatch in element size of parameter to be enqueued. */
            error = asl_fifo_error_param;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else {
            /* Success - perform enqueue. */
            const size_t index_read = fifo->index_read;
            const size_t index_write = fifo->index_write;
            const size_t free = available_write(index_read, index_write, k_count_max_elements);
            if ( !free ) {
                /* Found FIFO is full. */
                error = asl_fifo_error_full;
            } else {
                dest.ptr = fifo->elements.ptr + ( fifo->index_write * k_size_element );
                dest.size = k_size_element;
                /* Actual write. */
                asl_util_buffer_memcpy(dest, element);
                /* Moving write index ahead circularly. */
                fifo->index_write = circular_next(index_write, 1, k_count_max_elements);
                error = asl_fifo_error_ok;
            }
        }
    }
    return error;
}

static asl_fifo_error_n preview_dequeue(asl_fifo_t* ptr_fifo, asl_buffer_t* element, bool is_dequeue)
{
    asl_fifo_error_n error = asl_fifo_error_max;
    asl_fifo_t* const fifo = ptr_fifo;
    asl_buffer_t src;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !element || !element->ptr || !element->size ) {
        /* Found NULL or zero size in parameter. */
        error = asl_fifo_error_param;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = asl_fifo_error_illegal_state;
        } if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = asl_fifo_error_illegal_state;
        } else if ( k_size_element != element->size ) {
            /* Found mismatch in element size of parameter to be enqueued. */
            error = asl_fifo_error_param;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = asl_fifo_error_illegal_state;
        } else {
            /* Success - perform enqueue. */
            const size_t index_read = fifo->index_read;
            const size_t index_write = fifo->index_write;
            const size_t used = available_read(index_read, index_write, k_count_max_elements);
            if ( !used ) {
                /* Found FIFO is empty. */
                error = asl_fifo_error_full;
            } else {
                src.ptr = fifo->elements.ptr + ( fifo->index_read * k_size_element );
                src.size = k_size_element;
                /* Actual read. */
                asl_util_buffer_memcpy(*element, src);
                /* Moving read index ahead circularly, conditionally. */
                if ( is_dequeue ) {
                    fifo->index_read = circular_next(index_read, 1, k_count_max_elements);
                }
                error = asl_fifo_error_ok;
            }
        }
    }
    return error;
}

static size_t circular_next(size_t now, size_t step, size_t capacity)
{
    return ( ( now + step ) % capacity );
}

static size_t available_read(size_t index_read, size_t index_write, size_t capacity)
{
    return ( capacity - 1 - ( ( capacity - index_write + index_read - 1 ) % capacity ) );
}

static size_t available_write(size_t index_read, size_t index_write, size_t capacity)
{
    return ( capacity - index_write + index_read - 1 ) % capacity;
} 

