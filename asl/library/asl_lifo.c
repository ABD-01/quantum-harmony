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
 *  @date       17 October 2025
 *
 *  @brief      Implementation of the 'asl_lifo' Class.
*******************************************************************************/

/* Self. */
#include "asl_lifo.h"
#include "asl_lifo_priv.h"

/* ASL Util. */
#include "asl_util.h"

asl_lifo_error_e asl_lifo_reset(asl_lifo_s* ptr_lifo) {
    asl_lifo_error_e error = ASL_LIFO_E_MAX;
    asl_lifo_s* const lifo = ptr_lifo;
    asl_buffer_s paint;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_LIFO_E_PARAM;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = ASL_LIFO_E_ILLEGAL;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = ASL_LIFO_E_ILLEGAL;
        } else {
            /* Success - set the top to zero and paint the element slots and remainder area for debugging. */
            ptr_lifo->top = 0;
            for ( size_t i = 0 ; i < k_count_max_elements ; ++i ) {
                paint.ptr = lifo->elements.ptr + ( i * k_size_element );
                paint.size = k_size_element;
                asl_util_buffer_memset(paint, (uint8_t) i);
            }
            paint.ptr = lifo->elements.ptr + ( k_count_max_elements * k_size_element );
            paint.size = lifo->elements.size - ( k_count_max_elements * k_size_element );
            asl_util_buffer_memset(paint, (uint8_t) 0xFF);
            error = ASL_LIFO_E_OK;
        }
    }
    return error;
}

asl_lifo_error_e asl_lifo_get_count_capacity(asl_lifo_s* ptr_lifo, size_t* capacity) {
    asl_lifo_error_e error = ASL_LIFO_E_MAX;
    asl_lifo_s* const lifo = ptr_lifo;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !capacity ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_LIFO_E_PARAM;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = ASL_LIFO_E_ILLEGAL;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = ASL_LIFO_E_ILLEGAL;
        } else {
            /* Success - tell capacity count. */
            *capacity = k_count_max_elements;
            error = ASL_LIFO_E_OK;
        }
    }
    return error;
}

asl_lifo_error_e asl_lifo_get_count_used(asl_lifo_s* ptr_lifo, size_t* used) {
    asl_lifo_error_e error = ASL_LIFO_E_MAX;
    asl_lifo_s* const lifo = ptr_lifo;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !used ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_LIFO_E_PARAM;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = ASL_LIFO_E_ILLEGAL;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = ASL_LIFO_E_ILLEGAL;
        } else {
            /* Success - tell used slot count. */
            *used = lifo->top;
            error = ASL_LIFO_E_OK;
        }
    }
    return error;
}

asl_lifo_error_e asl_lifo_get_count_free(asl_lifo_s* ptr_lifo, size_t* free) {
    asl_lifo_error_e error = ASL_LIFO_E_MAX;
    asl_lifo_s* const lifo = ptr_lifo;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !free ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_LIFO_E_PARAM;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = ASL_LIFO_E_ILLEGAL;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = ASL_LIFO_E_ILLEGAL;
        } else {
            /* Success - tell free slot count. */
            *free = k_count_max_elements - lifo->top;
            error = ASL_LIFO_E_OK;
        }
    }
    return error;
}

asl_lifo_error_e asl_lifo_peek(asl_lifo_s* ptr_lifo, asl_buffer_s* element) {
    return peek_pop(ptr_lifo, element, false);
}

asl_lifo_error_e asl_lifo_pop(asl_lifo_s* ptr_lifo, asl_buffer_s* element) {
    return peek_pop(ptr_lifo, element, true);
}

asl_lifo_error_e asl_lifo_push(asl_lifo_s* ptr_lifo, asl_buffer_s element) {
    asl_lifo_error_e error = ASL_LIFO_E_MAX;
    asl_lifo_s* const lifo = ptr_lifo;
    asl_buffer_s dest;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !element.ptr || !element.size ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_LIFO_E_PARAM;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = ASL_LIFO_E_ILLEGAL;
        } else if ( k_size_element != element.size ) {
            /* Found mismatch in element size of parameter to be pushed. */
            error = ASL_LIFO_E_PARAM;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = ASL_LIFO_E_ILLEGAL;
        } else if ( lifo->top == k_count_max_elements ) {
            /* Found LIFO is full. */
            error = ASL_LIFO_E_FULL;
        } else {
            /* Success - push element and increment top. */
            dest.ptr = lifo->elements.ptr + ( lifo->top * k_size_element );
            dest.size = k_size_element;
            asl_util_buffer_memcpy(dest, element);
            lifo->top++;
            error = ASL_LIFO_E_OK;
        }
    }
    return error;
}

static asl_lifo_error_e peek_pop(asl_lifo_s* ptr_lifo, asl_buffer_s* element, bool is_pop) {
    asl_lifo_error_e error = ASL_LIFO_E_MAX;
    asl_lifo_s* const lifo = ptr_lifo;
    asl_buffer_s src;
    if ( !lifo || !lifo->elements.ptr || !lifo->elements.size || !lifo->size_element || !element || !element->ptr || !element->size ) {
        /* Found NULL or zero size in parameter. */
        error = ASL_LIFO_E_PARAM;
    } else {
        const size_t k_size_element = lifo->size_element;
        const size_t k_count_max_elements = lifo->elements.size / k_size_element;
        if ( 0 == k_count_max_elements ) {
            /* Covers the case of element size being bigger than size of entire LIFO memory. */
            error = ASL_LIFO_E_ILLEGAL;
        } else if ( k_size_element != element->size ) {
            /* Found mismatch in element size of parameter to be pushed. */
            error = ASL_LIFO_E_PARAM;
        } else if ( lifo->top > k_count_max_elements ) {
            /* Found top to be out-of-bounds for LIFO context specification. */
            error = ASL_LIFO_E_ILLEGAL;
        } else if ( lifo->top == 0 ) {
            /* Found LIFO is empty. */
            error = ASL_LIFO_E_EMPTY;
        } else {
            /* Success - push element and increment top. */
            src.ptr = lifo->elements.ptr + ( ( lifo->top - 1 ) * k_size_element );
            src.size = k_size_element;
            asl_util_buffer_memcpy(*element, src);
            if ( is_pop ) {
                lifo->top--;
            }
            error = ASL_LIFO_E_OK;
        }
    }
    return error;
} 
