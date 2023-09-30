/**
 * @file os_methods.h
 * @author Marcos Dominguez
 * @brief Private methods of the OS
 * @version 0.1
 * @date 2023-09-29
 *
 *
 */

#ifndef __OS_METHODS_H
#define __OS_METHODS_H

#include "OS/os_kernel.h"
/* =========[Definition of public macros] =========================================== */

/* =========[Definition of public data types] ======================================= */

/* =========[Definition of public methods]========================================== */

/**
 * @brief Set task as ready and push in fifo to be cheked by scheduler
 *
 * @param handler
 */
void OS_METHODS_SetTaskAsReady(os_task_t *handler);

/**
 * @brief Get a pointer to the current task.
 *
 * @return os_task_t*
 */
os_task_t*OS_METHODS_GetCurrentTask(void);

#endif /* __OS_METHODS_H */
