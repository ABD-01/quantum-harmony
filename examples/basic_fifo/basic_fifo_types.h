/*******************************************************************************
 *  @file       basic_fifo_types.h
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
 *  @brief      Types header for the 'basic_fifo' Example.
 *              Type definitions for the basic FIFO example implementation.
*******************************************************************************/

#if !defined(BASIC_FIFO_TYPES_H)
#define BASIC_FIFO_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ASL includes. */
#include "asl_fifo.h"

/* Public defines. */
#define BASIC_FIFO_DEFAULT_NUM_ELEMENTS     ( 10 )
#define BASIC_FIFO_DEFAULT_ELEMENT_SIZE     ( 4 )

/**
 * @brief
 * Context structure for basic FIFO example.
 */
typedef struct basic_fifo_context_s {
    /**
     * FIFO instance.
     */
    asl_fifo_s fifo;
    /**
     * Buffer memory for FIFO elements.
     */
    uint8_t* buffer_memory;
    /**
     * Size of each element in bytes.
     */
    size_t element_size;
    /**
     * Maximum number of elements.
     */
    size_t max_elements;
    /**
     * Test success flag.
     */
    bool test_passed;
} basic_fifo_context_s;

/**
 * @brief
 * Error codes for basic FIFO example.
 */
typedef enum basic_fifo_error_e {
    /**
     * No error.
     */
    BASIC_FIFO_E_OK = 0x00000001 << 0,
    /**
     * Parameter error.
     */
    BASIC_FIFO_E_PARAM = 0x00000001 << 1,
    /**
     * Memory allocation error.
     */
    BASIC_FIFO_E_MEMORY = 0x00000001 << 2,
    /**
     * Test failure error.
     */
    BASIC_FIFO_E_TEST_FAIL = 0x00000001 << 3,
    /**
     * Force 32-bit.
     */
    BASIC_FIFO_E_MAX = 0x7FFFFFFF
} basic_fifo_error_e;

#ifdef __cplusplus
}
#endif

#endif /* BASIC_FIFO_TYPES_H */
