/**
 * @file delay.h
 * @author Marcos Dominguez
 * @brief
 * @version 0.1
 * @date 2023-09-23
 *
 *
 */

#ifndef __DELAY_H
#define __DELAY_H

#include "OS/os_kernel.h"
#include "OS/os_arch_headers.h"

/* =========[Definition of public macros] =========================================== */

/* =========[Definition of public data types] ======================================= */

/* =========[Definition of public methods]========================================== */

/**
 * @brief Block task until the wake up time
 *
 * @param wake_up_time
 * @param task
 */
__STATIC_FORCEINLINE void DELAY_SetDelay(tick_type_t delay_time, os_task_t *task) {
    if (task != NULL) {
        if (delay_time == MAX_DELAY) {
            task->wake_up_time = MAX_DELAY;
        }
        else {
            task->wake_up_time = delay_time + OS_KERNEL_GetTickCount();
        }
        task->status = OS_TASK_BLOCK;
    }
}

/**
 * @brief Check if the delay is ready
 *
 * @param blocked_task
 */
__STATIC_FORCEINLINE bool DELAY_EvalDelay(os_task_t *task) {
    bool ret = false;
    if (task != NULL) {
        if ((OS_KERNEL_GetTickCount() >= task->wake_up_time) && (task->status == OS_TASK_BLOCK)) {
            ret = true;
        }
    }
    return ret;
}

#endif /* __DELAY_H */
