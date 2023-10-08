/**
 * @file lpc4337.h
 * @author Marcos Dominguez
 *
 * @brief Port for lpc4377 architecture. 
 * 
 * @version 0.1
 * @date 2023-10-08
 */

#ifndef INC_LPC4337_H_
#define INC_LPC4337_H_

#include "os_arch_headers.h"

#ifdef LPC4337

#include "cmsis_43xx.h"

/* =========[Definition of public macros] =========================================== */

#define IRQ_NUMBER      53                  ///< Number of interrupts supported by the MCU.

/* =========[Definition of public data types] ======================================= */

typedef LPC43XX_IRQn_Type os_irq_n_t;       ///< LPC43XX interrupt number definition.

#endif

#endif // INC_LPC4337_H_
