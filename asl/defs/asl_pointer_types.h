/*******************************************************************************
 *  @file       asl_pointer_types.h
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
 *  @brief      Header for the 'asl_pointer_types' Class.
 *              Basic pointer types derived from C99 standard types.
*******************************************************************************/

#if !defined(ASL_POINTER_TYPES_H)
#define ASL_POINTER_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief
 * Pointer to void.
 */
typedef void* pvoid;

/**
 * @brief
 * Pointer to size type.
 */
typedef size_t* psize_t;

/**
 * @brief
 * Pointer to unsigned 8-bit.
 */
typedef uint8_t* puint8_t;

/**
 * @brief
 * Pointer to unsigned 16-bit.
 */
typedef uint16_t* puint16_t;

/**
 * @brief
 * Pointer to unsigned 32-bit.
 */
typedef uint32_t* puint32_t;

/**
 * @brief
 * Pointer to unsigned 64-bit.
 */
typedef uint64_t* puint64_t;

/**
 * @brief
 * Pointer to constant void.
 */
typedef const void* pcvoid;

/**
 * @brief
 * Pointer to constant size type.
 */
typedef const size_t* pcsize_t;

/**
 * @brief
 * Pointer to constant unsigned 8-bit.
 */
typedef const uint8_t* pcuint8_t;

/**
 * @brief
 * Pointer to constant unsigned 16-bit.
 */
typedef const uint16_t* pcuint16_t;

/**
 * @brief
 * Pointer to constant unsigned 32-bit.
 */
typedef const uint32_t* pcuint32_t;

/**
 * @brief
 * Pointer to constant unsigned 64-bit.
 */
typedef const uint64_t* pcuint64_t;

#ifdef __cplusplus
}
#endif

#endif /* ASL_POINTER_TYPES_H */
