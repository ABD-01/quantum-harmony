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
 *  @version    0.0.1
 *
 *  @date       27 October 2025
 *
 *  @brief      Main entry point for basic CBUF example.
 *              Demonstrates simple producer-consumer pattern using ASL CBUF.
*******************************************************************************/

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* Example includes. */
#include "basic_cbuf.h"

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
 * Expected: program_name [cbuf_size]
 * 
 * @return
 * EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int main(int argc, char* argv[]) {
    basic_cbuf_context_s context = {0};
    basic_cbuf_error_e error = BASIC_CBUF_E_OK;
    size_t cbuf_size = BASIC_CBUF_DEFAULT_SIZE;
    int exit_code = EXIT_SUCCESS;
    
    printf("ASL Basic CBUF Example\n");
    printf("======================\n\n");
    
    /* Parse command line arguments */
    if (argc == 2) {
        cbuf_size = (size_t)atoi(argv[1]);
        
        if (cbuf_size == 0) {
            printf("Error: Invalid buffer size\n");
            basic_cbuf_print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    } else if (argc != 1) {
        basic_cbuf_print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    printf("CBUF size: %zu bytes\n\n", cbuf_size);
    
    /* Initialize, run test, cleanup */
    error = basic_cbuf_init(&context, cbuf_size);
    if (error == BASIC_CBUF_E_OK) {
        error = basic_cbuf_run_test(&context);
        basic_cbuf_print_results(&context);
        basic_cbuf_cleanup(&context);
    }
    
    if (error != BASIC_CBUF_E_OK) {
        printf("Example failed with error: 0x%08X\n", error);
        exit_code = EXIT_FAILURE;
    }
    
    return exit_code;
}
