#ifndef INC_LPC4337_H_
#define INC_LPC4337_H_

#include "os_arch_headers.h"

#ifdef LPC4337

#include "cmsis_43xx.h"

#define IRQ_NUMBER      53                  /* Number of interrupts supported by the MCU */

typedef LPC43XX_IRQn_Type   os_irq_n_t;     /* LPC43XX interrupt number definition */
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
void OS_IRQ_Handler(os_irq_n_t irq_type);

#endif // LPC4337
#endif // INC_LPC4337_H_

