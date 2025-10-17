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
 *  @date       17 October 2025
 *
 *  @brief      Implementation of the 'asl_fifo' Class.
*******************************************************************************/

/* Self. */
#include "asl_fifo.h"
#include "asl_fifo_priv.h"

/* ASL Util. */
#include "asl_util.h"

asl_fifo_error_e asl_fifo_reset(asl_fifo_s* ptr_fifo) {
    asl_fifo_error_e error = ASL_FIFO_E_MAX;
    asl_fifo_s* const fifo = ptr_fifo;
    asl_buffer_s paint;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_FIFO_E_PARAM;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = ASL_FIFO_E_ILLEGAL;
        } else {
            /* Success - set the top to zero and paint the element slots and remainder area for debugging. */
            fifo->index_read = 0;
            fifo->index_write = 0;
            for ( size_t i = 0 ; i < k_count_max_elements ; ++i ) {
                paint.ptr = fifo->elements.ptr + ( i * k_size_element );
                paint.size = k_size_element;
                asl_util_buffer_memset(paint, (uint8_t) i);
            }
            paint.ptr = fifo->elements.ptr + ( k_count_max_elements * k_size_element );
            paint.size = fifo->elements.size - ( k_count_max_elements * k_size_element );
            asl_util_buffer_memset(paint, (uint8_t) 0xFF);
            error = ASL_FIFO_E_OK;
        }
    }
    return error;
}

asl_fifo_error_e asl_fifo_get_count_capacity(asl_fifo_s* ptr_fifo, size_t* capacity) {
    asl_fifo_error_e error = ASL_FIFO_E_MAX;
    asl_fifo_s* const fifo = ptr_fifo;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !capacity ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_FIFO_E_PARAM;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else {
            /* Success - tell capacity count. */
            *capacity = k_count_max_elements - 1;
            error = ASL_FIFO_E_OK;
        }
    }
    return error;
}

asl_fifo_error_e asl_fifo_get_count_used(asl_fifo_s* ptr_fifo, size_t* used) {
    asl_fifo_error_e error = ASL_FIFO_E_MAX;
    asl_fifo_s* const fifo = ptr_fifo;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !used ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_FIFO_E_PARAM;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else {
            /* Success - tell used count. */
            const size_t index_read = fifo->index_read;
            const size_t index_write = fifo->index_write;
            *used = available_read(index_read, index_write, k_count_max_elements);
            error = ASL_FIFO_E_OK;
        }
    }
    return error;
}

asl_fifo_error_e asl_fifo_get_count_free(asl_fifo_s* ptr_fifo, size_t* free) {
    asl_fifo_error_e error = ASL_FIFO_E_MAX;
    asl_fifo_s* const fifo = ptr_fifo;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !free ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_FIFO_E_PARAM;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else {
            /* Success - tell free count. */
            const size_t index_read = fifo->index_read;
            const size_t index_write = fifo->index_write;
            *free = available_write(index_read, index_write, k_count_max_elements);
            error = ASL_FIFO_E_OK;
        }
    }
    return error;
}

asl_fifo_error_e asl_fifo_preview(asl_fifo_s* ptr_fifo, asl_buffer_s* element) {
    return preview_dequeue(ptr_fifo, element, false);
}

asl_fifo_error_e asl_fifo_dequeue(asl_fifo_s* ptr_fifo, asl_buffer_s* element) {
    return preview_dequeue(ptr_fifo, element, true);
}

asl_fifo_error_e asl_fifo_enqueue(asl_fifo_s* ptr_fifo, asl_buffer_s element) {
    asl_fifo_error_e error = ASL_FIFO_E_MAX;
    asl_fifo_s* const fifo = ptr_fifo;
    asl_buffer_s dest;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !element.ptr || !element.size ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_FIFO_E_PARAM;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( k_size_element != element.size ) {
            /* Found mismatch in element size of parameter to be enqueued. */
            error = ASL_FIFO_E_PARAM;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else {
            /* Success - perform enqueue. */
            const size_t index_read = fifo->index_read;
            const size_t index_write = fifo->index_write;
            const size_t free = available_write(index_read, index_write, k_count_max_elements);
            if ( !free ) {
                /* Found FIFO is full. */
                error = ASL_FIFO_E_FULL;
            } else {
                dest.ptr = fifo->elements.ptr + ( fifo->index_write * k_size_element );
                dest.size = k_size_element;
                /* Actual write. */
                asl_util_buffer_memcpy(dest, element);
                /* Moving write index ahead circularly. */
                fifo->index_write = circular_next(index_write, 1, k_count_max_elements);
                error = ASL_FIFO_E_OK;
            }
        }
    }
    return error;
}

static asl_fifo_error_e preview_dequeue(asl_fifo_s* ptr_fifo, asl_buffer_s* element, bool is_dequeue) {
    asl_fifo_error_e error = ASL_FIFO_E_MAX;
    asl_fifo_s* const fifo = ptr_fifo;
    asl_buffer_s src;
    if ( !fifo || !fifo->elements.ptr || !fifo->elements.size || !fifo->size_element || !element || !element->ptr || !element->size ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_FIFO_E_PARAM;
    } else {
        const size_t k_size_element = fifo->size_element;
        const size_t k_count_max_elements = fifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire FIFO memory. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( 1 == k_count_max_elements ) {
            /* Covers the case of single element FIFO which is of no use. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( k_size_element != element->size ) {
            /* Found mismatch in element size of parameter to be enqueued. */
            error = ASL_FIFO_E_PARAM;
        } else if ( fifo->index_read > k_count_max_elements ) {
            /* Read index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else if ( fifo->index_write > k_count_max_elements ) {
            /* Write index can never exceed the max elements. */
            error = ASL_FIFO_E_ILLEGAL;
        } else {
            /* Success - perform enqueue. */
            const size_t index_read = fifo->index_read;
            const size_t index_write = fifo->index_write;
            const size_t used = available_read(index_read, index_write, k_count_max_elements);
            if ( !used ) {
                /* Found FIFO is empty. */
                error = ASL_FIFO_E_EMPTY;
            } else {
                src.ptr = fifo->elements.ptr + ( fifo->index_read * k_size_element );
                src.size = k_size_element;
                /* Actual read. */
                asl_util_buffer_memcpy(*element, src);
                /* Moving read index ahead circularly, conditionally. */
                if ( is_dequeue ) {
                    fifo->index_read = circular_next(index_read, 1, k_count_max_elements);
                }
                error = ASL_FIFO_E_OK;
            }
        }
    }
    return error;
}

static size_t circular_next(size_t now, size_t step, size_t capacity) {
    return ( ( now + step ) % capacity );
}

static size_t available_read(size_t index_read, size_t index_write, size_t capacity) {
    return ( capacity - 1 - ( ( capacity - index_write + index_read - 1 ) % capacity ) );
}

static size_t available_write(size_t index_read, size_t index_write, size_t capacity) {
    return ( capacity - index_write + index_read - 1 ) % capacity;
}
