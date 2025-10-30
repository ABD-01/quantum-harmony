/*******************************************************************************
 *  @file       basic_cbuf_priv.h
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
 *  @brief      Private header for the 'basic_cbuf' Example.
 *              Internal declarations and helper functions.
*******************************************************************************/

#if !defined(BASIC_CBUF_PRIV_H)
#define BASIC_CBUF_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* Types includes. */
#include "basic_cbuf_types.h"

/* Private constants and macros. */
#define BASIC_CBUF_SLEEP_US             ( 1000 )

/**
 * @brief
 * Producer thread entry point.
 * 
 * @param
 * arg
 * Thread argument (context pointer).
 * 
 * @return
 * Thread return value.
 */
static void* basic_cbuf_producer_thread(void* arg);

/**
 * @brief
 * Consumer thread entry point.
 * 
 * @param
 * arg
 * Thread argument (context pointer).
 * 
 * @return
 * Thread return value.
 */
static void* basic_cbuf_consumer_thread(void* arg);

#ifdef __cplusplus
}
#endif

#endif /* BASIC_CBUF_PRIV_H */
