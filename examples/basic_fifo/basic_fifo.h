/*******************************************************************************
 *  @file       basic_fifo.h
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
 *  @brief      Header for the 'basic_fifo' Example.
 *              Basic FIFO example demonstrating enqueue/dequeue operations
 *              with configurable element size and boundary testing.
*******************************************************************************/

#if !defined(BASIC_FIFO_H)
#define BASIC_FIFO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types header. */
#include "basic_fifo_types.h"

/**
 * @brief
 * Initialize the basic FIFO example.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @param
 * num_elements
 * Number of elements the FIFO should hold.
 * 
 * @param
 * element_size
 * Size of each element in bytes.
 * 
 * @return
 * BASIC_FIFO_E_OK        - Success.
 * BASIC_FIFO_E_PARAM     - Invalid parameters.
 * BASIC_FIFO_E_MEMORY    - Memory allocation failure.
 */
basic_fifo_error_e basic_fifo_init(
    basic_fifo_context_s* ptr_context,
    size_t num_elements,
    size_t element_size
);

/**
 * @brief
 * Run the basic FIFO tests.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * BASIC_FIFO_E_OK        - All tests passed.
 * BASIC_FIFO_E_PARAM     - Invalid parameters.
 * BASIC_FIFO_E_TEST_FAIL - One or more tests failed.
 */
basic_fifo_error_e basic_fifo_run_tests(basic_fifo_context_s* ptr_context);

/**
 * @brief
 * Cleanup the basic FIFO example.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * BASIC_FIFO_E_OK        - Success.
 * BASIC_FIFO_E_PARAM     - Invalid parameters.
 */
basic_fifo_error_e basic_fifo_cleanup(basic_fifo_context_s* ptr_context);

/**
 * @brief
 * Print usage information.
 * 
 * @param
 * program_name
 * Name of the program executable.
 */
void basic_fifo_print_usage(const char* program_name);

/**
 * @brief
 * Print test results.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 */
void basic_fifo_print_results(basic_fifo_context_s* ptr_context);

#ifdef __cplusplus
}
#endif

#endif /* BASIC_FIFO_H */
