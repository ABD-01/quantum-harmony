/*******************************************************************************
 *  @file       basic_cbuf_types.h
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
 *  @brief      Types header for the 'basic_cbuf' Example.
 *              Type definitions for the basic CBUF example implementation.
*******************************************************************************/

#if !defined(BASIC_CBUF_TYPES_H)
#define BASIC_CBUF_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>

/* ASL includes. */
#include "asl_cbuf.h"

/* Default size of CBUF. */
#define BASIC_CBUF_DEFAULT_SIZE         ( 1024 )

/**
 * @brief
 * Context structure for basic CBUF example.
 */
typedef struct basic_cbuf_context_s {
    /**
     * Circular buffer instance.
     */
    asl_cbuf_s cbuf;
    /**
     * Buffer memory for CBUF.
     */
    uint8_t* buffer_memory;
    /**
     * Size of buffer memory.
     */
    size_t buffer_size;
    /**
     * Producer thread handle.
     */
    pthread_t producer_thread;
    /**
     * Consumer thread handle.
     */
    pthread_t consumer_thread;
    /**
     * Test completion flag.
     */
    volatile bool test_complete;
    /**
     * Test success flag.
     */
    bool test_passed;
    /**
     * Maximum usable capacity of the CBUF.
     */
    size_t max_capacity;
    /**
     * Current test size being executed.
     */
    size_t current_test_size;
    /**
     * Total number of successful tests.
     */
    size_t successful_tests;
    /**
     * Total number of failed tests.
     */
    size_t failed_tests;
} basic_cbuf_context_s;

/**
 * @brief
 * Error codes for basic CBUF example.
 */
typedef enum basic_cbuf_error_e {
    /**
     * No error.
     */
    BASIC_CBUF_E_OK = 0x00000001 << 0,
    /**
     * Parameter error.
     */
    BASIC_CBUF_E_PARAM = 0x00000001 << 1,
    /**
     * Memory allocation error.
     */
    BASIC_CBUF_E_MEMORY = 0x00000001 << 2,
    /**
     * Test failure error.
     */
    BASIC_CBUF_E_TEST_FAIL = 0x00000001 << 3,
    /**
     * Force 32-bit.
     */
    BASIC_CBUF_E_MAX = 0x7FFFFFFF
} basic_cbuf_error_e;

#ifdef __cplusplus
}
#endif

#endif /* BASIC_CBUF_TYPES_H */
