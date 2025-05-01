 /*******************************************************************************
 *  @file       asl_lifo.c
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
 *  @date       03-Apr-2025 20:21:03
 *
 *  @brief      Implementation of the 'asl_lifo' Class.
*******************************************************************************/

#include "asl_lifo.h"

/**
 * @brief
 * Pops or Peeks the element at top of the LIFO.
 * 
 * @param
 * ptr_lifo
 * Pointer to LIFO context.
 * 
 * @param
 * element
 * The element that is at top of stack, if stack is not empty.
 * 
 * @param
 * is_pop
 * Whether to actually Pop or just Peek.
 * 
 * @return
 * asl_lifo_error_ok            - Success.
 * asl_lifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_lifo_error_illegal_state - The operation cannot be performed because the LIFO state is corrupt.
 * asl_lifo_error_empty         - Operation is not possible since stack is empty.
 */
static asl_lifo_error_n peek_pop(asl_lifo_t* ptr_lifo, asl_buffer_t* element, bool is_pop);



asl_lifo_error_n asl_lifo_reset(asl_lifo_t* ptr_lifo)
{
    asl_lifo_error_n error = asl_lifo_error_max;
    asl_lifo_t* const lifo = ptr_lifo;
    asl_buffer_t paint;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element ) {
        /* Found NULL or zero size in parameter. */
        error = asl_lifo_error_param;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = asl_lifo_error_illegal_state;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = asl_lifo_error_illegal_state;
        } else {
            /* Success - set the top to zero and paint the element slots and remainder area for debugging. */
            ptr_lifo->top = 0;
            for ( size_t i = 0; i < k_count_max_elements ; ++i ) {
                paint.ptr = lifo->elements.ptr + ( i * k_size_element );
                paint.size = k_size_element;
                asl_util_buffer_memset(paint, (uint8_t) i);
            }
            paint.ptr = lifo->elements.ptr + ( k_count_max_elements * k_size_element );
            paint.size = lifo->elements.size - ( k_count_max_elements * k_size_element );
            asl_util_buffer_memset(paint, (uint8_t) 0xFF);
            error = asl_lifo_error_ok;
        }
    }
    return error;
}

asl_lifo_error_n asl_lifo_get_count_capacity(asl_lifo_t* ptr_lifo, size_t* capacity)
{
    asl_lifo_error_n error = asl_lifo_error_max;
    asl_lifo_t* const lifo = ptr_lifo;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !capacity ) {
        /* Found NULL or zero size in parameter. */
        error = asl_lifo_error_param;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = asl_lifo_error_illegal_state;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = asl_lifo_error_illegal_state;
        } else {
            /* Success - tell capacity count. */
            *capacity = k_count_max_elements;
            error = asl_lifo_error_ok;
        }
    }
    return error;
}

asl_lifo_error_n asl_lifo_get_count_used(asl_lifo_t* ptr_lifo, size_t* used)
{
    asl_lifo_error_n error = asl_lifo_error_max;
    asl_lifo_t* const lifo = ptr_lifo;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !used ) {
        /* Found NULL or zero size in parameter. */
        error = asl_lifo_error_param;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = asl_lifo_error_illegal_state;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = asl_lifo_error_illegal_state;
        } else {
            /* Success - tell used slot count. */
            *used = lifo->top;
            error = asl_lifo_error_ok;
        }
    }
    return error;
}

asl_lifo_error_n asl_lifo_get_count_free(asl_lifo_t* ptr_lifo, size_t* free)
{
    asl_lifo_error_n error = asl_lifo_error_max;
    asl_lifo_t* const lifo = ptr_lifo;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !free ) {
        /* Found NULL or zero size in parameter. */
        error = asl_lifo_error_param;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = asl_lifo_error_illegal_state;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = asl_lifo_error_illegal_state;
        } else {
            /* Success - tell free slot count. */
            *free = k_count_max_elements - lifo->top;
            error = asl_lifo_error_ok;
        }
    }
    return error;
}

asl_lifo_error_n asl_lifo_peek(asl_lifo_t* ptr_lifo, asl_buffer_t* element)
{
    return peek_pop(ptr_lifo, element, false);
}

asl_lifo_error_n asl_lifo_pop(asl_lifo_t* ptr_lifo, asl_buffer_t* element)
{
    return peek_pop(ptr_lifo, element, true);
}

asl_lifo_error_n asl_lifo_push(asl_lifo_t* ptr_lifo, asl_buffer_t element)
{
    asl_lifo_error_n error = asl_lifo_error_max;
    asl_lifo_t* const lifo = ptr_lifo;
    asl_buffer_t dest;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !element.ptr || !element.size ) {
        /* Found NULL or zero size in parameter. */
        error = asl_lifo_error_param;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = asl_lifo_error_illegal_state;
        } else if ( k_size_element != element.size ) {
            /* Found mismatch in element size of parameter to be pushed. */
            error = asl_lifo_error_param;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = asl_lifo_error_illegal_state;
        } else if ( lifo->top == k_count_max_elements ) {
            /* Found LIFO is full. */
            error = asl_lifo_error_full;
        } else {
            /* Success - push element and increment top. */
            dest.ptr = lifo->elements.ptr + ( lifo->top * k_size_element );
            dest.size = k_size_element;
            asl_util_buffer_memcpy(dest, element);
            lifo->top++;
            error = asl_lifo_error_ok;
        }
    }
    return error;
}

static asl_lifo_error_n peek_pop(asl_lifo_t* ptr_lifo, asl_buffer_t* element, bool is_pop)
{
    asl_lifo_error_n error = asl_lifo_error_max;
    asl_lifo_t* const lifo = ptr_lifo;
    asl_buffer_t src;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !element || !element->ptr || !element->size ) {
        /* Found NULL or zero size in parameter. */
        error = asl_lifo_error_param;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = asl_lifo_error_illegal_state;
        } else if ( k_size_element != element->size ) {
            /* Found mismatch in element size of parameter to be pushed. */
            error = asl_lifo_error_param;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = asl_lifo_error_illegal_state;
        } else if ( lifo->top == 0 ) {
            /* Found LIFO is empty. */
            error = asl_lifo_error_empty;
        } else {
            /* Success - push element and increment top. */
            src.ptr = lifo->elements.ptr + ( ( lifo->top - 1 ) * k_size_element );
            src.size = k_size_element;
            asl_util_buffer_memcpy(*element, src);
            if ( is_pop ) {
                lifo->top--;
            }
            error = asl_lifo_error_ok;
        }
    }
    return error;
} 

