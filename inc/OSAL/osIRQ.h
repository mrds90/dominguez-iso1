/**
 * @file osIRQ.h
 * @author Marcos Dominguez
 * 
 * @brief IRQ API.
 * 
 * @version 0.1
 * @date 2023-10-08
 */

#ifndef INC_OSIRQ_H_
#define INC_OSIRQ_H__

#include <stdbool.h>
#include "os_irq.h"

/* =========[Definition of public macros] =========================================== */

/* =========[Definition of public data types] ======================================= */

typedef os_irq_n_t osIRQnType; ///< Generic interrupt number definition.

/* =========[Definition of public methods]========================================== */

/**
 * @brief Registering the callback in the os interrupt vector and enabling the interrupt.
 *
 * @param[in]    irqType     IRQ number on the interrupts vector.
 * @param[in]    function    Logic to be executed in the interruption.
 * @param[in]    data        Data used by the logic performed in the interrupt.
 *
 * @return Returns true if the operation was successful otherwise false.
 */
#define osRegisterIRQ(irqType, function, data)     OS_IRQ_SetIRQ((osIRQnType) irqType, (IRQHandler) function, (void *)data)

/**
 * @brief Clears the callback register in the OS interrupt vector and disables the interrupt.
 *
 * @param[in]    irqType     IRQ number on the interrupts vector.
 *
 * @return Returns true if the operation was successful otherwise false.
 */
#define osUnregisterIRQ(irqType)                   OS_IRQ_ClearIRQ((osIRQnType) irqType)

#endif // INC_OSIRQ_H_
