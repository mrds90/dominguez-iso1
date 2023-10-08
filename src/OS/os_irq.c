#include "os_irq.h"
#include "os_methods.h"

/* ======================= Private data type ======================== */

/**
 * @brief Hold handler and context variable for each IRQ.
 */
typedef struct {
    IRQHandler handler;                     ///< Function served by the IRQ.
    void *data;                             ///< Data that is passed to the function that services the IRQ.
} os_irq_t;


/* ================== Private variables declaration ================= */

os_irq_t irq_vector[IRQ_NUMBER] = {0};      ///< Hold handler to be executed and context for each possible IRQ.

/* ================== Private functions declaration ================= */

/* ================= Public functions implementation ================ */

bool OS_IRQ_SetIRQ(os_irq_n_t irq_type, IRQHandler function, void *data) {
    bool ret = false;
    if ((irq_type >= Reset_IRQn) && (irq_type <= QEI_IRQn)) {
        irq_vector[irq_type].handler = function;
        irq_vector[irq_type].data = data;
        ret = true;
    }

    return ret;
}

bool OS_IRQ_ClearIRQ(os_irq_n_t irq_type) {
    bool ret = false;
    if ((irq_type >= Reset_IRQn) && (irq_type <= QEI_IRQn)) {
        irq_vector[irq_type].handler = NULL;
        irq_vector[irq_type].data = NULL;
        ret = true;
    }

    return ret;
}

void OS_IRQ_Handler(os_irq_n_t irq_type) {
    OS_METHODS_InterruptState(true);
    if (irq_vector[irq_type].handler != NULL) {
        irq_vector[irq_type].handler(irq_vector[irq_type].data);
    }
    OS_METHODS_InterruptState(false);
}
