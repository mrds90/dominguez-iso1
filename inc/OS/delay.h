/**
 * @file delay.h
 * @author Marcos Dominguez
 *
 * @brief Delay methods for the kernel (only used by the kernel).
 *
 * @version 0.1
 * @date 2023-09-23
 */

#ifndef __DELAY_H
#define __DELAY_H

#include "os_kernel.h"
#include "os_arch_headers.h"

/* =========[Definition of public macros] =========================================== */

/* =========[Definition of public data types] ======================================= */

/* =========[Definition of public methods]========================================== */

/**
 * @brief Block task until the wake up time.
 *
 * @param wake_up_time  Time to be blocked.
 * @param task          Task to be blocked.
 */
__STATIC_FORCEINLINE void DELAY_SetDelay(tick_type_t delay_time, os_task_t *task) {
    if (task != NULL) {
        if ((MAX_DELAY - delay_time) <=  OS_KERNEL_GetTickCount()) { // if wake up time will be higher than MAX_DELAY so set MAX_DELAY as delay.
            task->wake_up_time = MAX_DELAY;
        }
        else {
            task->wake_up_time = delay_time + OS_KERNEL_GetTickCount();
        }
        task->status = OS_TASK_BLOCK;
    }
}

/**
 * @brief Check if the delay is ready.
 *
 * @param task  Task to check if is ready or blocked.
 */
__STATIC_FORCEINLINE bool DELAY_EvalDelay(os_task_t *task) {
    bool ret = false;
    if (task != NULL && (task->wake_up_time != MAX_DELAY)) {
        if ((OS_KERNEL_GetTickCount() >= task->wake_up_time) && (task->status == OS_TASK_BLOCK)) {
            ret = true;
        }
    }
    return ret;
}

#endif /* __DELAY_H */
