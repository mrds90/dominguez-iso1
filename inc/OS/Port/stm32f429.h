#ifndef INC_STM32F429_H_
#define INC_STM32F429_H_

#ifdef STM32F429

/**
 * @note THe macros, types definition, variables and method of this file must be used internaly in the OS.
 */
#include "stm32f429xx.h"

#define IRQ_NUMBER      91                  /* Number of interrupts supported by the MCU */

typedef IRQn_Type   os_irq_n_t;             /* STM32F4XX interrupt number definition */
typedef void (*IRQHandler)(void* data);     /* Protorype of function */

// Data type of IRQ vector.
typedef struct
{
	IRQHandler  handler;    // Function served by the IRQ.
	void*       data;		// Data that is passed to the function that services the IRQ.
}os_irq_t;


/**
 * @brief Function used to execute the interrupt logic and
 * clear the interrupt trigger bit.
 *
 * @param[in]	irqType		IRQ number on the interrupts vector.
 */
void OS_IRQ_Handler(os_irq_n_t irqType);

#endif // STM32F429
#endif // INC_PORTSTM32F429ZI_H_

