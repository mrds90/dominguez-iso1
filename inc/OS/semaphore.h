/**
 * @file semaphore.h
 * @author Marcos Dominguez
 * @brief
 * @version 0.1
 * @date 2023-09-28
 *
 *
 */

#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include "OS/os_kernel.h"
/* =========[Definition of public macros] =========================================== */

/* =========[Definition of public data types] ======================================= */

typedef enum {
    SEMAPHORE_STATE_TAKEN,
    SEMAPHORE_STATE_FREE,

    SEMAPHORE_STATE_QTY,
} semaphore_state_t;

typedef struct
{
    os_task_t *task_list[MAX_NUMBER_TASK];
    os_task_t **pop_task;
    os_task_t **push_task;
    semaphore_state_t status;
} semaphore_t;

/* =========[Definition of public methods]========================================== */


bool SEMAPHORE_CreateBinary(semaphore_t *semph);

void SEMAPHORE_Give(semaphore_t *semph);

bool SEMAPHORE_Take(semaphore_t *semph, const tick_type_t wait_time);

#endif /* __SEMAPHORE_H */
