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
 *  @date       03-Apr-2025 20:21:03
 *
 *  @brief      Header for the 'asl_types' Class.
*******************************************************************************/

#if !defined(ASL_TYPES_H)
#define ASL_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "asl_pointer_types.h"
#include "asl_iface_types.h"

/**
 * @brief
 * Encapsulated abstractions of commonly used simple derived types.
 */

/**
 * @brief
 * Encapsulated abstraction of bounded memory (location and size).
 */
typedef struct asl_buffer_t
{
    /**
     * Pointer to memory.
     */
    puint8_t ptr;
    /**
     * Size of pointed memory.
     */
    size_t size;
}  asl_buffer_t;


/**
 * @brief
 * Encapsulated abstraction of dynamic memory allocation.
 */
typedef struct asl_allocator_t
{
    /**
     * Allocates requested amount of memory and returns pointer to it.
     * If memory could not be allocated, NULL is returned.
     */
    asl_iface_pv_s_t alloc_f;
    /**
     * Release previously allocated memory.
     */
    asl_iface_v_pv_t free_f;
}  asl_allocator_t;


/**
 * @brief
 * Encapsulated abstraction of mutual exclusion (lock/unlock) primitives.
 */
typedef struct asl_mutex_t
{
    /**
     * Take lock.
     */
    asl_iface_v_v_t lock_f;
    /**
     * Release lock.
     */
    asl_iface_v_v_t unlock_f;
}  asl_mutex_t;


#ifdef __cplusplus
}
#endif


#endif /* ASL_TYPES_H */
