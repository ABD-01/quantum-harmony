/*******************************************************************************
 *  @file       asl_iface_types.h
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
 *  @brief      Header for the 'asl_iface_types' Class.
 *              This file is created by EA, it should practically hold 'typedef' alias to function pointers.
 *              The 'typedef' aliases can be used in EA UML as a 'delegate class', thus enabling their use as UML attribute.
 *              Care has been taken by the UML architect (author) to define 'function pointer' and 'typedef' with exact same name.
 *              Hence, from the C compiler standpoint, the 'typedef' does not need to be defined here, it can simply use the function
 *              pointer prototypes directly.
 *              Thus this file should contain nothing other than the inclusion of the actual file where function pointer prototypes are
 *              present.
*******************************************************************************/

#if !defined(ASL_IFACE_TYPES_H)
#define ASL_IFACE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ASL includes. */
#include "asl_iface_priv_types.h"

#ifdef __cplusplus
}
#endif

#endif /* ASL_IFACE_TYPES_H */
