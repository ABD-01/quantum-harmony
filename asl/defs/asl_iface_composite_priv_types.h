/*******************************************************************************
 *  @file       asl_iface_composite_priv_types.h
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
 *  @brief      Header for the 'asl_iface_composite_priv_types' Class.
 *              Composite function pointer prototypes.
*******************************************************************************/

#if !defined(ASL_IFACE_COMPOSITE_PRIV_TYPES_H)
#define ASL_IFACE_COMPOSITE_PRIV_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ASL includes. */
#include "asl_types.h"

/**
 * @brief
 * Function prototype that transmits/receives serial data.
 */
typedef size_t (*asl_iface_composite_rx_tx_t)(asl_buffer_s buffer, size_t req);

#ifdef __cplusplus
}
#endif

#endif /* ASL_IFACE_COMPOSITE_PRIV_TYPES_H */
