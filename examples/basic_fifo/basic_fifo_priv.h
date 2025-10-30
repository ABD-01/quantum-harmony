/*******************************************************************************
 *  @file       basic_fifo_priv.h
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
 *  @brief      Private header for the 'basic_fifo' Example.
 *              Internal declarations and helper functions.
*******************************************************************************/

#if !defined(BASIC_FIFO_PRIV_H)
#define BASIC_FIFO_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Types includes. */
#include "basic_fifo_types.h"

/* Private constants and macros. */
#define BASIC_FIFO_MIN_ELEMENT_SIZE         ( 1 )
#define BASIC_FIFO_MAX_ELEMENT_SIZE         ( 1024 )
#define BASIC_FIFO_MIN_NUM_ELEMENTS         ( 1 )
#define BASIC_FIFO_MAX_NUM_ELEMENTS         ( 10000 )

/**
 * @brief
 * Test empty FIFO condition.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * Error code.
 */
static basic_fifo_error_e basic_fifo_test_empty(basic_fifo_context_s* ptr_context);

/**
 * @brief
 * Test full FIFO condition.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * Error code.
 */
static basic_fifo_error_e basic_fifo_test_full(basic_fifo_context_s* ptr_context);

/**
 * @brief
 * Fill FIFO to capacity with test data.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @param
 * capacity
 * FIFO capacity.
 * 
 * @param
 * test_element
 * Buffer for test element.
 * 
 * @return
 * Error code.
 */
static basic_fifo_error_e basic_fifo_fill_to_capacity(
    basic_fifo_context_s* ptr_context, 
    size_t capacity, 
    uint8_t* test_element
);

/**
 * @brief
 * Test enqueue to full FIFO (should fail).
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @param
 * element_buffer
 * Test element buffer.
 * 
 * @return
 * Error code.
 */
static basic_fifo_error_e basic_fifo_test_enqueue_full(
    basic_fifo_context_s* ptr_context, 
    asl_buffer_s element_buffer
);

/**
 * @brief
 * Dequeue all elements and verify data integrity.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @param
 * capacity
 * FIFO capacity.
 * 
 * @param
 * received_element
 * Buffer for received element.
 * 
 * @return
 * Error code.
 */
static basic_fifo_error_e basic_fifo_dequeue_and_verify(
    basic_fifo_context_s* ptr_context, 
    size_t capacity, 
    uint8_t* received_element
);

#ifdef __cplusplus
}
#endif

#endif /* BASIC_FIFO_PRIV_H */
