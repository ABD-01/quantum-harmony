/*******************************************************************************
 *  @file       asl_lifo_priv.h
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
 *  @brief      Private header for the 'asl_lifo' Class.
*******************************************************************************/

#if !defined(ASL_LIFO_PRIV_H)
#define ASL_LIFO_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types header. */
#include "asl_lifo_types.h"

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
 * ASL_LIFO_E_OK            - Success.
 * ASL_LIFO_E_PARAM         - NULL pointer or zero sized buffer or other parameter inconsistency.
 * ASL_LIFO_E_ILLEGAL       - The operation cannot be performed because the LIFO state is corrupt.
 * ASL_LIFO_E_EMPTY         - Operation is not possible since stack is empty.
 */
static asl_lifo_error_e peek_pop(asl_lifo_s* ptr_lifo, asl_buffer_s* element, bool is_pop);

#ifdef __cplusplus
}
#endif

#endif /* ASL_LIFO_PRIV_H */
