/*******************************************************************************
 *  @file       asl_cbuf_priv.h
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
 *  @brief      Private header for the 'asl_cbuf' Class.
*******************************************************************************/

#if !defined(ASL_CBUF_PRIV_H)
#define ASL_CBUF_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Types includes. */
#include "asl_cbuf_types.h"

/* Enable parameter checking (optimization macro). */
#define ASL_CBUF_ENABLE_PARAM_CHECK ( 1 )

/**
 * @brief
 * Performs a 'circular next' from current index.
 * 
 * @param
 * now
 * The queue index from where to begin.
 * 
 * @param
 * step
 * The number of steps to jump.
 * 
 * @param
 * capacity
 * The capacity of backing buffer (i.e. size of backing buffer).
 * 
 * @return
 * The index after performing circular next.
 */
static size_t circular_next(size_t now, size_t step, size_t capacity);

/**
 * @brief
 * Tells number of elements that are already present.
 * 
 * @param
 * front
 * Current front (index) of the queue.
 * 
 * @param
 * rear
 * Current rear (index) of the queue.
 * 
 * @param
 * capacity
 * The capacity of backing buffer (i.e. size of backing buffer).
 * 
 * @return
 * The number of elements that are already present.
 */
static size_t available_read(size_t front, size_t rear, size_t capacity);

/**
 * @brief
 * Tells max number of elements that can be written.
 * 
 * @param
 * front
 * Current front (index) of the queue.
 * 
 * @param
 * rear
 * Current rear (index) of the queue.
 * 
 * @param
 * capacity
 * The capacity of backing buffer (i.e. size of backing buffer).
 * 
 * @return
 * Max number of elements that can be written.
 */
static size_t available_write(size_t front, size_t rear, size_t capacity);

#ifdef __cplusplus
}
#endif

#endif /* ASL_CBUF_PRIV_H */
