/**
 * @file stm32f429.h
 * @author Marcos Dominguez
 *
 * @brief Port for stm32f429 architecture.
 *
 * @version 0.1
 * @date 2023-10-08
 */

#ifndef INC_STM32F429_H_
#define INC_STM32F429_H_

#ifdef STM32F429

#include "stm32f429xx.h"

/* =========[Definition of public macros] =========================================== */

#define IRQ_NUMBER      91                  ///< Number of interrupts supported by the MCU.

/* =========[Definition of public data types] ======================================= */

typedef IRQn_Type os_irq_n_t;               ///< STM32F4XX interrupt number definition.

#endif // STM32F429
#endif // INC_PORTSTM32F429ZI_H_
