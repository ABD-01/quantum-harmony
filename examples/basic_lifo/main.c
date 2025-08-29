/*******************************************************************************
 *  @file       main.c
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
 *  @brief      Main entry point for basic LIFO example.
 *              Demonstrates LIFO push/pop operations with configurable
 *              element size and boundary condition testing.
*******************************************************************************/

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* Example includes. */
#include "basic_lifo.h"

/**
 * @brief
 * Application entry point.
 * 
 * @param
 * argc
 * Number of command line arguments.
 * 
 * @param
 * argv
 * Array of command line argument strings.
 * Expected: program_name <num_elements> <element_size>
 * 
 * @return
 * EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int main(int argc, char* argv[]) {
    basic_lifo_context_s context = {0};
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    size_t num_elements = BASIC_LIFO_DEFAULT_NUM_ELEMENTS;
    size_t element_size = BASIC_LIFO_DEFAULT_ELEMENT_SIZE;
    int exit_code = EXIT_SUCCESS;
    
    printf("ASL Basic LIFO Example\n");
    printf("======================\n\n");
    
    // Parse command line arguments
    if (argc == 3) {
        num_elements = (size_t)atoi(argv[1]);
        element_size = (size_t)atoi(argv[2]);
        
        if (num_elements == 0 || element_size == 0) {
            printf("Error: Invalid parameters\n");
            basic_lifo_print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    } else if (argc != 1) {
        basic_lifo_print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    printf("LIFO Configuration:\n");
    printf("  Elements: %zu\n", num_elements);
    printf("  Element size: %zu bytes\n\n", element_size);
    
    // Initialize, run tests, cleanup
    error = basic_lifo_init(&context, num_elements, element_size);
    if (error == BASIC_LIFO_E_OK) {
        error = basic_lifo_run_tests(&context);
        basic_lifo_print_results(&context);
        basic_lifo_cleanup(&context);
    }
    
    if (error != BASIC_LIFO_E_OK) {
        printf("Example failed with error: 0x%08X\n", error);
        exit_code = EXIT_FAILURE;
    }
    
    return exit_code;
}
