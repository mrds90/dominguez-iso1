/**
 * @file os_delay.h
 * @author Marcos Dominguez
 *
 * @brief Delay methods for the kernel (only used by the kernel).
 *
 * @version 0.1
 * @date 2023-09-23
 */

#ifndef __OS_DELAY_H
#define __OS_DELAY_H

#include "os_kernel.h"
#include "os_arch_headers.h"

/* ========= [Definition of public macros] =========================================== */

/* ========= [Definition of public data types] ======================================= */

/* ========= [Definition of public methods] ========================================== */

/**
 * @brief Block task until the wake up time.
 *
 * @param wake_up_time  Time to be blocked.
 * @param task          Task to be blocked.
 */
__STATIC_FORCEINLINE void OS_DELAY_SetDelay(tick_type_t delay_time, os_task_t *task) {
    if (task != NULL) {
        if (((OS_MAX_DELAY - delay_time) <=  OS_KERNEL_GetTickCount()) || (delay_time > OS_MAX_DELAY)) { // if wake up time will be higher than OS_MAX_DELAY so set OS_MAX_DELAY as delay.
            task->wake_up_time = OS_MAX_DELAY;
        }
        else {
            task->wake_up_time = delay_time + OS_KERNEL_GetTickCount();
        }
        if (task->status == OS_TASK_RUNNING) {
            task->status = OS_TASK_BLOCK;
        }
    }
}

/**
 * @brief Check if the delay is ready.
 *
 * @param task  Task to check if is ready or blocked.
 */
__STATIC_FORCEINLINE bool OS_DELAY_EvalDelay(os_task_t *task) {
    bool ret = false;
    if (task != NULL && (task->wake_up_time != OS_MAX_DELAY)) {
        if ((OS_KERNEL_GetTickCount() >= task->wake_up_time) && (task->status == OS_TASK_BLOCK)) {
            ret = true;
        }
    }
    return ret;
}

#endif /* __OS_DELAY_H */
