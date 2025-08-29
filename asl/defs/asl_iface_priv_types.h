/*******************************************************************************
 *  @file       asl_iface_priv_types.h
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
 *  @brief      Header for the 'asl_iface_priv_types' Class.
*******************************************************************************/

#if !defined(ASL_IFACE_PRIV_TYPES_H)
#define ASL_IFACE_PRIV_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ASL includes. */
#include "asl_pointer_types.h"

/**
 * @brief
 * Basic function pointer prototypes.
 */

typedef void (*asl_iface_v_v_cb)(void);

typedef void (*asl_iface_v_pv_cb)(pvoid arg);

typedef void (*asl_iface_v_s_cb)(size_t arg);

typedef void (*asl_iface_v_ps_cb)(psize_t arg);

typedef void (*asl_iface_v_u8_cb)(uint8_t arg);

typedef void (*asl_iface_v_pu8_cb)(puint8_t arg);

typedef void (*asl_iface_v_u16_cb)(uint16_t arg);

typedef void (*asl_iface_v_pu16_cb)(puint16_t arg);

typedef void (*asl_iface_v_u32_cb)(uint32_t arg);

typedef void (*asl_iface_v_pu32_cb)(puint32_t arg);

typedef void (*asl_iface_v_u64_cb)(uint64_t arg);

typedef void (*asl_iface_v_pu64_cb)(puint64_t arg);

typedef pvoid (*asl_iface_pv_v_cb)(void);

typedef pvoid (*asl_iface_pv_pv_cb)(pvoid arg);

typedef pvoid (*asl_iface_pv_s_cb)(size_t arg);

typedef pvoid (*asl_iface_pv_ps_cb)(psize_t arg);

typedef pvoid (*asl_iface_pv_u8_cb)(uint8_t arg);

typedef pvoid (*asl_iface_pv_pu8_cb)(puint8_t arg);

typedef pvoid (*asl_iface_pv_u16_cb)(uint16_t arg);

typedef pvoid (*asl_iface_pv_pu16_cb)(puint16_t arg);

typedef pvoid (*asl_iface_pv_u32_cb)(uint32_t arg);

typedef pvoid (*asl_iface_pv_pu32_cb)(puint32_t arg);

typedef pvoid (*asl_iface_pv_u64_cb)(uint64_t arg);

typedef pvoid (*asl_iface_pv_pu64_cb)(puint64_t arg);

typedef size_t (*asl_iface_s_v_cb)(void);

typedef size_t (*asl_iface_s_pv_cb)(pvoid arg);

typedef size_t (*asl_iface_s_s_cb)(size_t arg);

typedef size_t (*asl_iface_s_ps_cb)(psize_t arg);

typedef size_t (*asl_iface_s_u8_cb)(uint8_t arg);

typedef size_t (*asl_iface_s_pu8_cb)(puint8_t arg);

typedef size_t (*asl_iface_s_u16_cb)(uint16_t arg);

typedef size_t (*asl_iface_s_pu16_cb)(puint16_t arg);

typedef size_t (*asl_iface_s_u32_cb)(uint32_t arg);

typedef size_t (*asl_iface_s_pu32_cb)(puint32_t arg);

typedef size_t (*asl_iface_s_u64_cb)(uint64_t arg);

typedef size_t (*asl_iface_s_pu64_cb)(puint64_t arg);

typedef psize_t (*asl_iface_ps_v_cb)(void);

typedef psize_t (*asl_iface_ps_pv_cb)(pvoid arg);

typedef psize_t (*asl_iface_ps_s_cb)(size_t arg);

typedef psize_t (*asl_iface_ps_ps_cb)(psize_t arg);

typedef psize_t (*asl_iface_ps_u8_cb)(uint8_t arg);

typedef psize_t (*asl_iface_ps_pu8_cb)(puint8_t arg);

typedef psize_t (*asl_iface_ps_u16_cb)(uint16_t arg);

typedef psize_t (*asl_iface_ps_pu16_cb)(puint16_t arg);

typedef psize_t (*asl_iface_ps_u32_cb)(uint32_t arg);

typedef psize_t (*asl_iface_ps_pu32_cb)(puint32_t arg);

typedef psize_t (*asl_iface_ps_u64_cb)(uint64_t arg);

typedef psize_t (*asl_iface_ps_pu64_cb)(puint64_t arg);

typedef uint8_t (*asl_iface_u8_v_cb)(void);

typedef uint8_t (*asl_iface_u8_pv_cb)(pvoid arg);

typedef uint8_t (*asl_iface_u8_s_cb)(size_t arg);

typedef uint8_t (*asl_iface_u8_ps_cb)(psize_t arg);

typedef uint8_t (*asl_iface_u8_u8_cb)(uint8_t arg);

typedef uint8_t (*asl_iface_u8_pu8_cb)(puint8_t arg);

typedef uint8_t (*asl_iface_u8_u16_cb)(uint16_t arg);

typedef uint8_t (*asl_iface_u8_pu16_cb)(puint16_t arg);

typedef uint8_t (*asl_iface_u8_u32_cb)(uint32_t arg);

typedef uint8_t (*asl_iface_u8_pu32_cb)(puint32_t arg);

typedef uint8_t (*asl_iface_u8_u64_cb)(uint64_t arg);

typedef uint8_t (*asl_iface_u8_pu64_cb)(puint64_t arg);

typedef puint8_t (*asl_iface_pu8_v_cb)(void);

typedef puint8_t (*asl_iface_pu8_pv_cb)(pvoid arg);

typedef puint8_t (*asl_iface_pu8_s_cb)(size_t arg);

typedef puint8_t (*asl_iface_pu8_ps_cb)(psize_t arg);

typedef puint8_t (*asl_iface_pu8_u8_cb)(uint8_t arg);

typedef puint8_t (*asl_iface_pu8_pu8_cb)(puint8_t arg);

typedef puint8_t (*asl_iface_pu8_u16_cb)(uint16_t arg);

typedef puint8_t (*asl_iface_pu8_pu16_cb)(puint16_t arg);

typedef puint8_t (*asl_iface_pu8_u32_cb)(uint32_t arg);

typedef puint8_t (*asl_iface_pu8_pu32_cb)(puint32_t arg);

typedef puint8_t (*asl_iface_pu8_u64_cb)(uint64_t arg);

typedef puint8_t (*asl_iface_pu8_pu64_cb)(puint64_t arg);

typedef uint16_t (*asl_iface_u16_v_cb)(void);

typedef uint16_t (*asl_iface_u16_pv_cb)(pvoid arg);

typedef uint16_t (*asl_iface_u16_s_cb)(size_t arg);

typedef uint16_t (*asl_iface_u16_ps_cb)(psize_t arg);

typedef uint16_t (*asl_iface_u16_u8_cb)(uint8_t arg);

typedef uint16_t (*asl_iface_u16_pu8_cb)(puint8_t arg);

typedef uint16_t (*asl_iface_u16_u16_cb)(uint16_t arg);

typedef uint16_t (*asl_iface_u16_pu16_cb)(puint16_t arg);

typedef uint16_t (*asl_iface_u16_u32_cb)(uint32_t arg);

typedef uint16_t (*asl_iface_u16_pu32_cb)(puint32_t arg);

typedef uint16_t (*asl_iface_u16_u64_cb)(uint64_t arg);

typedef uint16_t (*asl_iface_u16_pu64_cb)(puint64_t arg);

typedef puint16_t (*asl_iface_pu16_v_cb)(void);

typedef puint16_t (*asl_iface_pu16_pv_cb)(pvoid arg);

typedef puint16_t (*asl_iface_pu16_s_cb)(size_t arg);

typedef puint16_t (*asl_iface_pu16_ps_cb)(psize_t arg);

typedef puint16_t (*asl_iface_pu16_u8_cb)(uint8_t arg);

typedef puint16_t (*asl_iface_pu16_pu8_cb)(puint8_t arg);

typedef puint16_t (*asl_iface_pu16_u16_cb)(uint16_t arg);

typedef puint16_t (*asl_iface_pu16_pu16_cb)(puint16_t arg);

typedef puint16_t (*asl_iface_pu16_u32_cb)(uint32_t arg);

typedef puint16_t (*asl_iface_pu16_pu32_cb)(puint32_t arg);

typedef puint16_t (*asl_iface_pu16_u64_cb)(uint64_t arg);

typedef puint16_t (*asl_iface_pu16_pu64_cb)(puint64_t arg);

typedef uint32_t (*asl_iface_u32_v_cb)(void);

typedef uint32_t (*asl_iface_u32_pv_cb)(pvoid arg);

typedef uint32_t (*asl_iface_u32_s_cb)(size_t arg);

typedef uint32_t (*asl_iface_u32_ps_cb)(psize_t arg);

typedef uint32_t (*asl_iface_u32_u8_cb)(uint8_t arg);

typedef uint32_t (*asl_iface_u32_pu8_cb)(puint8_t arg);

typedef uint32_t (*asl_iface_u32_u16_cb)(uint16_t arg);

typedef uint32_t (*asl_iface_u32_pu16_cb)(puint16_t arg);

typedef uint32_t (*asl_iface_u32_u32_cb)(uint32_t arg);

typedef uint32_t (*asl_iface_u32_pu32_cb)(puint32_t arg);

typedef uint32_t (*asl_iface_u32_u64_cb)(uint64_t arg);

typedef uint32_t (*asl_iface_u32_pu64_cb)(puint64_t arg);

typedef puint32_t (*asl_iface_pu32_v_cb)(void);

typedef puint32_t (*asl_iface_pu32_pv_cb)(pvoid arg);

typedef puint32_t (*asl_iface_pu32_s_cb)(size_t arg);

typedef puint32_t (*asl_iface_pu32_ps_cb)(psize_t arg);

typedef puint32_t (*asl_iface_pu32_u8_cb)(uint8_t arg);

typedef puint32_t (*asl_iface_pu32_pu8_cb)(puint8_t arg);

typedef puint32_t (*asl_iface_pu32_u16_cb)(uint16_t arg);

typedef puint32_t (*asl_iface_pu32_pu16_cb)(puint16_t arg);

typedef puint32_t (*asl_iface_pu32_u32_cb)(uint32_t arg);

typedef puint32_t (*asl_iface_pu32_pu32_cb)(puint32_t arg);

typedef puint32_t (*asl_iface_pu32_u64_cb)(uint64_t arg);

typedef puint32_t (*asl_iface_pu32_pu64_cb)(puint64_t arg);

typedef uint64_t (*asl_iface_u64_v_cb)(void);

typedef uint64_t (*asl_iface_u64_pv_cb)(pvoid arg);

typedef uint64_t (*asl_iface_u64_s_cb)(size_t arg);

typedef uint64_t (*asl_iface_u64_ps_cb)(psize_t arg);

typedef uint64_t (*asl_iface_u64_u8_cb)(uint8_t arg);

typedef uint64_t (*asl_iface_u64_pu8_cb)(puint8_t arg);

typedef uint64_t (*asl_iface_u64_u16_cb)(uint16_t arg);

typedef uint64_t (*asl_iface_u64_pu16_cb)(puint16_t arg);

typedef uint64_t (*asl_iface_u64_u32_cb)(uint32_t arg);

typedef uint64_t (*asl_iface_u64_pu32_cb)(puint32_t arg);

typedef uint64_t (*asl_iface_u64_u64_cb)(uint64_t arg);

typedef uint64_t (*asl_iface_u64_pu64_cb)(puint64_t arg);

typedef puint64_t (*asl_iface_pu64_v_cb)(void);

typedef puint64_t (*asl_iface_pu64_pv_cb)(pvoid arg);

typedef puint64_t (*asl_iface_pu64_s_cb)(size_t arg);

typedef puint64_t (*asl_iface_pu64_ps_cb)(psize_t arg);

typedef puint64_t (*asl_iface_pu64_u8_cb)(uint8_t arg);

typedef puint64_t (*asl_iface_pu64_pu8_cb)(puint8_t arg);

typedef puint64_t (*asl_iface_pu64_u16_cb)(uint16_t arg);

typedef puint64_t (*asl_iface_pu64_pu16_cb)(puint16_t arg);

typedef puint64_t (*asl_iface_pu64_u32_cb)(uint32_t arg);

typedef puint64_t (*asl_iface_pu64_pu32_cb)(puint32_t arg);

typedef puint64_t (*asl_iface_pu64_u64_cb)(uint64_t arg);

typedef puint64_t (*asl_iface_pu64_pu64_cb)(puint64_t arg);

#ifdef __cplusplus
}
#endif

#endif /* ASL_IFACE_PRIV_TYPES_H */
