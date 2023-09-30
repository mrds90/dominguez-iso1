/**
 * @file semaphore.c
 * @author Marcos Dominguez
 * @brief
 * @version 0.1
 * @date 2023-09-28
 *
 *
 */

#include "OS/semaphore.h"
#include "OS/delay.h"
#include "OS/os_methods.h"

/* ==================== Define private variables ==================== */

/* ================== Private variables declaration ================= */

/* ================== Private functions declaration ================= */

/* ================= Public functions implementation ================ */

bool SEMAPHORE_CreateBinary(semaphore_t *semph) {
    semph->pop_task = semph->task_list;
    semph->push_task = semph->task_list;
    semph->status = SEMAPHORE_STATE_TAKEN;
}

void SEMAPHORE_Give(semaphore_t *semph) {
    if (semph->pop_task != NULL) {
        if (semph->status == SEMAPHORE_STATE_TAKEN) {
            semph->status = SEMAPHORE_STATE_FREE;
            if (semph->pop_task != semph->push_task) {
                os_task_t *task_handler = *semph->pop_task;
                *semph->pop_task++;
                if (semph->pop_task == &semph->task_list[MAX_NUMBER_TASK]) {
                    semph->pop_task = semph->task_list;
                }
                OS_METHODS_SetTaskAsReady(task_handler);
                OS_KERNEL_PortYield();
            }
        }
    }
}

bool SEMAPHORE_Take(semaphore_t *semph, const tick_type_t wait_time) {
    bool ret = false;
    if (semph->pop_task != NULL) {
        NVIC_DisableIRQ(SysTick_IRQn);
        ret = true;
        if (semph->status == SEMAPHORE_STATE_TAKEN) {
            os_task_t *current_task = OS_METHODS_GetCurrentTask();
            current_task->status = OS_TASK_BLOCK;
            *semph->push_task = current_task;

            semph->push_task++;
            if (semph->push_task == &semph->task_list[MAX_NUMBER_TASK]) {
                semph->push_task = semph->task_list;
            }
            OS_KERNEL_Delay(wait_time);
            if (semph->status == SEMAPHORE_STATE_TAKEN) {
                ret = false;
            }
        }
        semph->status = SEMAPHORE_STATE_TAKEN;

        NVIC_EnableIRQ(SysTick_IRQn);
    }
    return ret;
}

/* ================ Private functions implementation ================ */

/* ========== Processor Interruption and Exception Handlers ========= */
