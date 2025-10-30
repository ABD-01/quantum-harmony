/*******************************************************************************
 *  @file       basic_lifo.h
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
 *  @brief      Header for the 'basic_lifo' Example.
 *              Basic LIFO example demonstrating push/pop operations
 *              with configurable element size and boundary testing.
*******************************************************************************/

#if !defined(BASIC_LIFO_H)
#define BASIC_LIFO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types header. */
#include "basic_lifo_types.h"

/**
 * @brief
 * Initialize the basic LIFO example.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @param
 * num_elements
 * Number of elements the LIFO should hold.
 * 
 * @param
 * element_size
 * Size of each element in bytes.
 * 
 * @return
 * BASIC_LIFO_E_OK        - Success.
 * BASIC_LIFO_E_PARAM     - Invalid parameters.
 * BASIC_LIFO_E_MEMORY    - Memory allocation failure.
 */
basic_lifo_error_e basic_lifo_init(
    basic_lifo_context_s* ptr_context,
    size_t num_elements,
    size_t element_size
);

/**
 * @brief
 * Run the basic LIFO tests.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * BASIC_LIFO_E_OK        - All tests passed.
 * BASIC_LIFO_E_PARAM     - Invalid parameters.
 * BASIC_LIFO_E_TEST_FAIL - One or more tests failed.
 */
basic_lifo_error_e basic_lifo_run_tests(basic_lifo_context_s* ptr_context);

/**
 * @brief
 * Cleanup the basic LIFO example.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * BASIC_LIFO_E_OK        - Success.
 * BASIC_LIFO_E_PARAM     - Invalid parameters.
 */
basic_lifo_error_e basic_lifo_cleanup(basic_lifo_context_s* ptr_context);

/**
 * @brief
 * Print usage information.
 * 
 * @param
 * program_name
 * Name of the program executable.
 */
void basic_lifo_print_usage(const char* program_name);

/**
 * @brief
 * Print test results.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 */
void basic_lifo_print_results(basic_lifo_context_s* ptr_context);

#ifdef __cplusplus
}
#endif

#endif /* BASIC_LIFO_H */
