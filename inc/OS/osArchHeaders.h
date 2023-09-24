/**
 * @file osArchHeader.h
 * @author Marcos Dominguez
 * @brief
 * @version 0.1
 * @date 2023-09-24
 *
 *
 */

#ifndef __OS_ARCH_HEADERS_H
#define __OS_ARCH_HEADERS_H

#ifdef ARCH_NXP_LPC_43XX
#include "cmsis.h"
#include "chip.h"
#elif ARCH_STM_32_F4XX
#include "system_stm32f4xx.h"
#include "stm32f429xx.h"
#include "core_cm4.h"
#endif

#include "cmsis_gcc.h"
/* =========[Definition of public macros] =========================================== */

/* =========[Definition of public data types] ======================================= */

/* =========[Definition of public methods]========================================== */


#endif /* __OS_ARCH_HEADERS_H */
