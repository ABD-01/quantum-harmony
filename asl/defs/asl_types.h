/*******************************************************************************
 *  @file       asl_types.h
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
 *  @date       17 October 2025
 *
 *  @brief      Header for the 'asl_types' Class.
 *              Encapsulated abstractions of commonly used simple derived types.
*******************************************************************************/

#if !defined(ASL_TYPES_H)
#define ASL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ASL includes. */
#include "asl_pointer_types.h"
#include "asl_iface_types.h"

/**
 * @brief
 * Encapsulated abstraction of bounded memory (location and size).
 */
typedef struct asl_buffer_s
{
    /**
     * Pointer to memory.
     */
    puint8_t ptr;
    /**
     * Size of pointed memory.
     */
    size_t size;
} asl_buffer_s;

/**
 * @brief
 * Encapsulated abstraction of dynamic memory allocation.
 */
typedef struct asl_allocator_s
{
    /**
     * Allocates requested amount of memory and returns pointer to it.
     * If memory could not be allocated, NULL is returned.
     */
    asl_iface_pv_s_cb alloc_f;
    /**
     * Release previously allocated memory.
     */
    asl_iface_v_pv_cb free_f;
} asl_allocator_s;

/**
 * @brief
 * Encapsulated abstraction of mutual exclusion (lock/unlock) primitives.
 */
typedef struct asl_mutex_s
{
    /**
     * Take lock.
     */
    asl_iface_v_v_cb lock_f;
    /**
     * Release lock.
     */
    asl_iface_v_v_cb unlock_f;
} asl_mutex_s;

#ifdef __cplusplus
}
#endif

#endif /* ASL_TYPES_H */
