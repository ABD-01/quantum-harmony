/*******************************************************************************
 *  @file       basic_cbuf.h
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
 *  @brief      Header for the 'basic_cbuf' Example.
 *              Basic CBUF example demonstrating lockless producer-consumer
 *              pattern with simple threading.
*******************************************************************************/

#if !defined(BASIC_CBUF_H)
#define BASIC_CBUF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types header. */
#include "basic_cbuf_types.h"

/**
 * @brief
 * Initialize the basic CBUF example.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @param
 * cbuf_size
 * Size of the circular buffer.
 * 
 * @return
 * BASIC_CBUF_E_OK        - Success.
 * BASIC_CBUF_E_PARAM     - Invalid parameters.
 * BASIC_CBUF_E_MEMORY    - Memory allocation failure.
 */
basic_cbuf_error_e basic_cbuf_init(basic_cbuf_context_s* ptr_context, size_t cbuf_size);

/**
 * @brief
 * Run the basic CBUF test.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * BASIC_CBUF_E_OK        - Test passed.
 * BASIC_CBUF_E_PARAM     - Invalid parameters.
 * BASIC_CBUF_E_TEST_FAIL - Test failed.
 */
basic_cbuf_error_e basic_cbuf_run_test(basic_cbuf_context_s* ptr_context);

/**
 * @brief
 * Cleanup the basic CBUF example.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 * 
 * @return
 * BASIC_CBUF_E_OK        - Success.
 * BASIC_CBUF_E_PARAM     - Invalid parameters.
 */
basic_cbuf_error_e basic_cbuf_cleanup(basic_cbuf_context_s* ptr_context);

/**
 * @brief
 * Print usage information.
 * 
 * @param
 * program_name
 * Name of the program executable.
 */
void basic_cbuf_print_usage(const char* program_name);

/**
 * @brief
 * Print test results.
 * 
 * @param
 * ptr_context
 * Pointer to example context.
 */
void basic_cbuf_print_results(basic_cbuf_context_s* ptr_context);

#ifdef __cplusplus
}
#endif

#endif /* BASIC_CBUF_H */
