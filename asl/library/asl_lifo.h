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
 *  @date       27 November 2025
 *
 *  @brief      Header for the 'asl_lifo' Class.
 *              A generic LIFO.
*******************************************************************************/

#if !defined(ASL_LIFO_H)
#define ASL_LIFO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types header. */
#include "asl_lifo_types.h"

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
 * ASL_LIFO_E_OK            - Success.
 * ASL_LIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_LIFO_E_ILLEGAL       - The operation cannot be performed because the LIFO state is corrupt.
 */
asl_lifo_error_e asl_lifo__reset(asl_lifo_s* ptr_lifo);

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
 * ASL_LIFO_E_OK            - Success.
 * ASL_LIFO_E_PARAM         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * ASL_LIFO_E_ILLEGAL       - The operation cannot be performed because the LIFO state is corrupt.
 */
asl_lifo_error_e asl_lifo__get_count_capacity(asl_lifo_s* ptr_lifo, size_t* capacity);

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
 * ASL_LIFO_E_OK            - Success.
 * ASL_LIFO_E_PARAM         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * ASL_LIFO_E_ILLEGAL       - The operation cannot be performed because the LIFO state is corrupt.
 */
asl_lifo_error_e asl_lifo__get_count_used(asl_lifo_s* ptr_lifo, size_t* used);

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
 * ASL_LIFO_E_OK            - Success.
 * ASL_LIFO_E_PARAM         - NULL pointers or zero sized buffer or other parameter inconsistency.
 * ASL_LIFO_E_ILLEGAL       - The operation cannot be performed because the LIFO state is corrupt.
 */
asl_lifo_error_e asl_lifo__get_count_free(asl_lifo_s* ptr_lifo, size_t* free);

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
 * ASL_LIFO_E_OK            - Success.
 * ASL_LIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_LIFO_E_ILLEGAL       - The operation cannot be performed because the LIFO state is corrupt.
 * ASL_LIFO_E_EMPTY         - Operation is not possible since stack is empty.
 */
asl_lifo_error_e asl_lifo__peek(asl_lifo_s* ptr_lifo, asl_buffer_s* element);

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
 * ASL_LIFO_E_OK            - Success.
 * ASL_LIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_LIFO_E_ILLEGAL       - The operation cannot be performed because the LIFO state is corrupt.
 * ASL_LIFO_E_EMPTY         - Operation is not possible since stack is empty.
 */
asl_lifo_error_e asl_lifo__pop(asl_lifo_s* ptr_lifo, asl_buffer_s* element);

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
 * ASL_LIFO_E_OK            - Success.
 * ASL_LIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_LIFO_E_ILLEGAL       - The operation cannot be performed because the LIFO state is corrupt.
 * ASL_LIFO_E_FULL          - Operation is not possible since stack is full.
 */
asl_lifo_error_e asl_lifo__push(asl_lifo_s* ptr_lifo, asl_buffer_s element);

#ifdef __cplusplus
}
#endif

#endif /* ASL_LIFO_H */
