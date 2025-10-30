/*******************************************************************************
 *  @file       basic_lifo_priv.h
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
 *  @date       27 October 2025
 *
 *  @brief      Private header for the 'basic_lifo' Example.
 *              Internal declarations and helper functions.
*******************************************************************************/

#if !defined(BASIC_LIFO_PRIV_H)
#define BASIC_LIFO_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Types includes. */
#include "basic_lifo_types.h"

/* Private constants and macros. */
#define BASIC_LIFO_MIN_ELEMENT_SIZE         ( 1 )
#define BASIC_LIFO_MAX_ELEMENT_SIZE         ( 1024 )
#define BASIC_LIFO_MIN_NUM_ELEMENTS         ( 1 )
#define BASIC_LIFO_MAX_NUM_ELEMENTS         ( 10000 )

/**
 * @brief
 * Test empty LIFO condition.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * Error code.
 */
static basic_lifo_error_e basic_lifo_test_empty(basic_lifo_context_s* ptr_context);

/**
 * @brief
 * Test full LIFO condition.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * Error code.
 */
static basic_lifo_error_e basic_lifo_test_full(basic_lifo_context_s* ptr_context);

/**
 * @brief
 * Fill LIFO to capacity with test pattern data.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * test_element
 * Buffer for element data.
 * element_buffer
 * Buffer structure for ASL API.
 * 
 * @return
 * Error code.
 */
static basic_lifo_error_e basic_lifo_fill_to_capacity(
    basic_lifo_context_s* ptr_context,
    uint8_t* test_element,
    asl_buffer_s* element_buffer);

/**
 * @brief
 * Verify LIFO full state and test boundary conditions.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * test_element
 * Buffer for element data.
 * element_buffer
 * Buffer structure for ASL API.
 * 
 * @return
 * Error code.
 */
static basic_lifo_error_e basic_lifo_verify_full_state(
    basic_lifo_context_s* ptr_context,
    uint8_t* test_element,
    asl_buffer_s* element_buffer);

/**
 * @brief
 * Test peek operation from full LIFO.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * peek_element
 * Buffer for peeked element data.
 * element_buffer
 * Buffer structure for ASL API.
 * 
 * @return
 * Error code.
 */
static basic_lifo_error_e basic_lifo_test_peek_full(
    basic_lifo_context_s* ptr_context,
    uint8_t* peek_element,
    asl_buffer_s* element_buffer);

/**
 * @brief
 * Pop all elements from LIFO and verify data integrity.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * peek_element
 * Buffer for popped element data.
 * element_buffer
 * Buffer structure for ASL API.
 * 
 * @return
 * Error code.
 */
static basic_lifo_error_e basic_lifo_pop_all_elements(
    basic_lifo_context_s* ptr_context,
    uint8_t* peek_element,
    asl_buffer_s* element_buffer);

#ifdef __cplusplus
}
#endif

#endif /* BASIC_LIFO_PRIV_H */
