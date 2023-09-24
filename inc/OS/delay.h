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

#include "OS/osKernel.h"
#include "OS/osArchHeaders.h"

/* =========[Definition of public macros] =========================================== */

/* =========[Definition of public data types] ======================================= */

/* =========[Definition of public methods]========================================== */

/**
 * @brief Block task until the wake up time
 *
 * @param wake_up_time
 * @param task
 */
__STATIC_FORCEINLINE void DELAY_SetDelay(tick_type_t delay_time, osTaskObject *task) {
    task->wake_up_time = delay_time + osGetTickCount();
    task->status = OS_TASK_BLOCK;
}

/**
 * @brief Check if the is ready
 *
 * @param blocked_task
 */
__STATIC_FORCEINLINE void DELAY_EvalDelay(osTaskObject *task) {
    if ((osGetTickCount() >= task->wake_up_time) && (task->status == OS_TASK_BLOCK)) {
        task->status = OS_TASK_READY;
    }
}

#endif /* __DELAY_H */