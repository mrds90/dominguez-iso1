/**
 * @file os_arch_headers.h
 * @author Marcos Dominguez
 * 
 * @brief Port for supported architectures.
 * 
 * @version 0.1
 * @date 2023-09-24
 */

#ifndef __OS_ARCH_HEADERS_H
#define __OS_ARCH_HEADERS_H

#ifdef ARCH_NXP_LPC_43XX
#include "cmsis.h"
#include "chip.h"
#define LPC4337
#else
#ifdef ARCH_STM_32_F4XX
#include "stm32f429xx.h"
#include "system_stm32f4xx.h"
#include "core_cm4.h"
#define STM32F429
#endif
#endif

#include "cmsis_gcc.h"

/* =========[Definition of public macros] =========================================== */

/* =========[Definition of public data types] ======================================= */

/* =========[Definition of public methods]========================================== */


#endif /* __OS_ARCH_HEADERS_H */
