/*******************************************************************************
 *  @file       asl_lifo.h
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
 *  @brief      Header for the 'asl_lifo' Class.
*******************************************************************************/

#if !defined(ASL_LIFO_H)
#define ASL_LIFO_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "asl_types.h"
#include "asl_pointer_types.h"
#include "asl_util.h"

/**
 * @brief
 * A generic LIFO.
 */

/**
 * @brief
 * LIFO context definition.
 * Number of elements is inferred from elements.size / size_element and cannot evaluate to zero.
 */
typedef struct asl_lifo_t
{
    /**
     * Size in bytes of a single element in the LIFO.
     */
    size_t size_element;
    /**
     * Buffer that stores actual elements of LIFO.
     */
    asl_buffer_t elements;
    /**
     * Zero base relative 'top' of stack.
     * When [ 'top' == 0 ] - it means stack is empty (no elements).
     * When [ 'top' == elements.size/size_element ] - it means stack is full.
     */
    size_t top;
}  asl_lifo_t;


/**
 * @brief
 * LIFO API error codes.
 */
typedef enum asl_lifo_error_n
{
    /**
     * No error.
     */
    asl_lifo_error_ok = 0x00000001 << 0,
    /**
     * Parameter is incorrect (mostly for NULL check).
     */
    asl_lifo_error_param = 0x00000001 << 1,
    /**
     * The LIFO context is in illegal state.
     */
    asl_lifo_error_illegal_state = 0x00000001 << 2,
    /**
     * The LIFO pop/peek is not possible since LIFO is empty.
     */
    asl_lifo_error_empty = 0x00000001 << 3,
    /**
     * The LIFO push is not possible since LIFO is full.
     */
    asl_lifo_error_full = 0x00000001 << 4,
    /**
     * Force 32-bit.
     */
    asl_lifo_error_max = 0x7FFFFFFF
} asl_lifo_error_n;

/**
 * @brief
 * Resets the LIFO context (debug paints 'elements', sets 'top' to zero).
 * It is recommended (but not strictly required) to do LIFO reset before first use.
 * 
 * @param
 * ptr_lifo
 * Pointer to LIFO context.
 * 
 * @return
 * asl_lifo_error_ok            - Success.
 * asl_lifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_lifo_error_illegal_state - The operation cannot be performed because the LIFO state is corrupt.
 */
asl_lifo_error_n asl_lifo_reset(asl_lifo_t* ptr_lifo);

/**
 * @brief
 * Gets the capacity (maximum number of elements) possible in the LIFO.
 * 
 * @param
 * ptr_lifo
 * Pointer to LIFO context.
 * 
 * @param
 * capacity
 * The maximum number of elements possible in the LIFO.
 * 
 * @return
 * asl_lifo_error_ok            - Success.
 * asl_lifo_error_param         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * asl_lifo_error_illegal_state - The operation cannot be performed because the LIFO state is corrupt.
 */
asl_lifo_error_n asl_lifo_get_count_capacity(asl_lifo_t* ptr_lifo, size_t* capacity);

/**
 * @brief
 * Gets the used number of element slots in the LIFO.
 * 
 * @param
 * ptr_lifo
 * Pointer to LIFO context.
 * 
 * @param
 * used
 * The used number of element slots in the LIFO.
 * 
 * @return
 * asl_lifo_error_ok            - Success.
 * asl_lifo_error_param         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * asl_lifo_error_illegal_state - The operation cannot be performed because the LIFO state is corrupt.
 */
asl_lifo_error_n asl_lifo_get_count_used(asl_lifo_t* ptr_lifo, size_t* used);

/**
 * @brief
 * Gets the free number of element slots in the LIFO.
 * 
 * @param
 * ptr_lifo
 * Pointer to LIFO context.
 * 
 * @param
 * free
 * The free number of element slots in the LIFO.
 * 
 * @return
 * asl_lifo_error_ok            - Success.
 * asl_lifo_error_param         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * asl_lifo_error_illegal_state - The operation cannot be performed because the LIFO state is corrupt.
 */
asl_lifo_error_n asl_lifo_get_count_free(asl_lifo_t* ptr_lifo, size_t* free);

/**
 * @brief
 * Peeks at element at top of the LIFO.
 * This operation does not pop the element.
 * 
 * @param
 * ptr_lifo
 * Pointer to LIFO context.
 * 
 * @param
 * element
 * The element that is at top of stack, if stack is not empty.
 * 
 * @return
 * asl_lifo_error_ok            - Success.
 * asl_lifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_lifo_error_illegal_state - The operation cannot be performed because the LIFO state is corrupt.
 * asl_lifo_error_empty         - Operation is not possible since stack is empty.
 */
asl_lifo_error_n asl_lifo_peek(asl_lifo_t* ptr_lifo, asl_buffer_t* element);

/**
 * @brief
 * Pops the element at top of the LIFO.
 * 
 * @param
 * ptr_lifo
 * Pointer to LIFO context.
 * 
 * @param
 * element
 * The element that is at top of stack, if stack is not empty.
 * 
 * @return
 * asl_lifo_error_ok            - Success.
 * asl_lifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_lifo_error_illegal_state - The operation cannot be performed because the LIFO state is corrupt.
 * asl_lifo_error_empty         - Operation is not possible since stack is empty.
 */
asl_lifo_error_n asl_lifo_pop(asl_lifo_t* ptr_lifo, asl_buffer_t* element);

/**
 * @brief
 * Pushes an element into the LIFO.
 * 
 * @param
 * ptr_lifo
 * Pointer to LIFO context.
 * 
 * @param
 * element
 * The element that needs to be pushed.
 * 
 * @return
 * asl_lifo_error_ok            - Success.
 * asl_lifo_error_param         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * asl_lifo_error_illegal_state - The operation cannot be performed because the LIFO state is corrupt.
 * asl_lifo_error_full          - Operation is not possible since stack is full.
 */
asl_lifo_error_n asl_lifo_push(asl_lifo_t* ptr_lifo, asl_buffer_t element);


#ifdef __cplusplus
}
#endif


#endif /* ASL_LIFO_H */
