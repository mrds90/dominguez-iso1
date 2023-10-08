#ifndef INC_OSIRQ_H_
#define INC_OSIRQ_H__

#include <stdbool.h>
#include "os_arch_headers.h"

#ifdef STM32F429
#include "stm32f429.h"
#endif

#ifdef LPC4337
#include "lpc4337.h"
#endif



/**
 * @brief Registering the callback in the os interrupt vector and enabling the interrupt.
 *
 * @param[in]	irq_type		IRQ number on the interrupts vector.
 * @param[in]	function    Logic to be executed in the interruption.
 * @param[in]	data		Data used by the logic performed in the interrupt.
 *
 * @return Returns true if the operation was successful otherwise false.
 */
bool OS_IRQ_SetIRQ(os_irq_n_t irq_type, IRQHandler function, void *data);

/**
 * @brief Clears the callback register in the OS interrupt vector and disables the interrupt.
 *
 * @param[in]	irq_type		IRQ number on the interrupts vector.
 *
 * @return Returns true if the operation was successful otherwise false.
 */
bool OS_IRQ_ClearIRQ(os_irq_n_t irq_type);

#endif // INC_OSIRQ_H_

