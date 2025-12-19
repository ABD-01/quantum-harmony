/*******************************************************************************
 *  @file       basic_lifo_types.h
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
 *  @brief      Types header for the 'basic_lifo' Example.
 *              Type definitions for the basic LIFO example implementation.
*******************************************************************************/

#if !defined(BASIC_LIFO_TYPES_H)
#define BASIC_LIFO_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ASL includes. */
#include "asl_lifo.h"

/* Public defines. */
#define BASIC_LIFO_DEFAULT_NUM_ELEMENTS     ( 10 )
#define BASIC_LIFO_DEFAULT_ELEMENT_SIZE     ( 4 )

/**
 * @brief
 * Context structure for basic LIFO example.
 */
typedef struct basic_lifo_context_s {
    /**
     * LIFO instance.
     */
    asl_lifo_s lifo;
    /**
     * Buffer memory for LIFO elements.
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
} basic_lifo_context_s;

/**
 * @brief
 * Error codes for basic LIFO example.
 */
typedef enum basic_lifo_error_e {
    /**
     * No error.
     */
    BASIC_LIFO_E_OK = 0x00000001 << 0,
    /**
     * Parameter error.
     */
    BASIC_LIFO_E_PARAM = 0x00000001 << 1,
    /**
     * Memory allocation error.
     */
    BASIC_LIFO_E_MEMORY = 0x00000001 << 2,
    /**
     * Test failure error.
     */
    BASIC_LIFO_E_TEST_FAIL = 0x00000001 << 3,
    /**
     * Force 32-bit.
     */
    BASIC_LIFO_E_MAX = 0x7FFFFFFF
} basic_lifo_error_e;

#ifdef __cplusplus
}
#endif

#endif /* BASIC_LIFO_TYPES_H */
