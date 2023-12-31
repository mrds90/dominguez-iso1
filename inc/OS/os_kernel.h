/**
 * @file os_kernel.h
 * @author Marcos Dominguez
 *
 * @brief Kernel class atributes and public methods.
 *
 * @version 0.1
 * @date 2023-10-08
 */

#ifndef __OS_KERNEL_H_
#define __OS_KERNEL_H_

#include <stdint.h>
#include <stdbool.h>
#include "os_config.h"

/* ========= [Definition of public macros] =========================================== */

#define MAX_TASK_SIZE           (MAX_STACK_SIZE / sizeof(uint32_t))     ///< Defines maximum stack size for a task.
#define OS_MAX_DELAY            0xFFFFFFFFFFFFFFFFUL                    ///< Macro defining an infinite delay for waiting forever.
#define SET_PRIORITY(x)         (((uint32_t)(x) >= PRIORITY_LEVELS) ? priority_map[PRIORITY_LEVELS - 1] : priority_map[x]) ///< Safe way to set priority

/* ========= [Definition of public data types] ======================================= */

typedef uint64_t tick_type_t;       ///< Tick data type.

/**
 * @brief Available priorities for the task.
 */
typedef enum {
    OS_TASK_READY,                  ///< Ready state.
    OS_TASK_RUNNING,                ///< Running state.
    OS_TASK_BLOCK,                  ///< Blocked state.
    OS_TASK_SUSPEND                 ///< Suspended state.
} os_task_status_t;

/**
 * @brief Kernel Priorities.
 */
typedef enum
{
    OS_KERNEL_LOW_PRIORITY,         ///< Idle task priority (LOWEST).
    #if (PRIORITY_LEVELS > 1)
    OS_KERNEL_NORMAL_PRIORITY,      ///< Normal priority level.
    #if (PRIORITY_LEVELS > 2)
    OS_KERNEL_HIGH_PRIORITY,        ///< High priority level.
    #if (PRIORITY_LEVELS > 3)
    OS_KERNEL_VERYHIGH_PRIORITY,    ///< Highest priority level.
    #endif /* (PRIORITY_LEVELS > 3) */
    #endif /* (PRIORITY_LEVELS > 2) */
    #endif /* (PRIORITY_LEVELS > 1) */

    OS_KERNEL_PRIORITY_QTY,
} os_priority_t;

/**
 * @brief Map of kernel priorities that reverse priority levels (0 as the highest priority).
 */
static const os_priority_t priority_map[OS_KERNEL_PRIORITY_QTY] = {
    #if (PRIORITY_LEVELS > 3)
    [(OS_KERNEL_PRIORITY_QTY - OS_KERNEL_VERYHIGH_PRIORITY) - 1] = OS_KERNEL_VERYHIGH_PRIORITY,
    #endif
    #if (PRIORITY_LEVELS > 2)
    [(OS_KERNEL_PRIORITY_QTY - OS_KERNEL_HIGH_PRIORITY) - 1] = OS_KERNEL_HIGH_PRIORITY,
    #endif
    #if (PRIORITY_LEVELS > 1)
    [(OS_KERNEL_PRIORITY_QTY - OS_KERNEL_NORMAL_PRIORITY) - 1] = OS_KERNEL_NORMAL_PRIORITY,
    #endif
    [(OS_KERNEL_PRIORITY_QTY - OS_KERNEL_LOW_PRIORITY) - 1] = OS_KERNEL_LOW_PRIORITY,
};

/**
 * @brief Atributes of the task class (Used by the kernel).
 */
typedef struct {
    uint32_t memory[MAX_TASK_SIZE];          ///< Memory stack.
    uint32_t stack_pointer;                  ///< Stack pointer of task.
    void *entry_point;                       ///< Callback executed on task.
    uintptr_t id;                            ///< Task ID, it's a memory position.
    os_task_status_t status;                 ///< Status task.
    os_task_status_t prev_status;            ///< Previous status of the task.
    os_priority_t priority;                  ///< Task priority.
    tick_type_t wake_up_time;                ///< Time to unblock task if blocked.
} os_task_t;

/* ========= [Definition of public methods] ======================================== */

/**
 *@brief Create task.
 *
 *@param[in, out]   handler  Data structure of task.
 *@param[in]        priority Task priority level.
 *@param[in]        callback Function executed on task.
 *
 *@return Return true if task was success or false in otherwise.
 */
bool OS_KERNEL_TaskCreate(os_task_t *handler, os_priority_t priority, void *callback);

/**
 *@brief Delete task removing from kernel task list.
 *
 *@param[in, out]   handler Data structure of task, if NULL will delete current task.
 */
void OS_KERNEL_TaskDelete(os_task_t *handler);

/**
 *@brief Suspend task removing from kernel waiting task FIFOs.
 *
 *@param[in, out]   handler Data structure of task, if NULL will delete current task.
 */
void OS_KERNEL_TaskSuspend(os_task_t *handler);

/**
 *@brief Resume task restoring suspended task.
 *
 *@param[in, out]   handler Data structure of task, if NULL will delete current task.
 */
void OS_KERNEL_TaskResume(os_task_t *handler);

/**
 * @brief Initialization pendSV exception with lowest priority possible.
 */
void OS_KERNEL_Start(void);

/**
 * @brief Get Tick count of the OS.
 *
 * @return Ticks counted since OS started.
 */
tick_type_t OS_KERNEL_GetTickCount(void);

/**
 * @brief Execute a delay for the current task.
 *
 * @param[in]   tick Number ticks delayed.
 */
void OS_KERNEL_Delay(const tick_type_t tick);

/**
 * @brief Declare the end of the critical section.
 */
void OS_KERNEL_EnterCritical(void);

/**
 * @brief Declare the beginning of the critical section.
 */
void OS_KERNEL_ExitCritical(void);

/**
 * @brief Function used as default when some task return for a problem.
 */
void OS_KERNEL_ReturnTaskHook(void);

/**
 * @brief Function used if user would like to do something on Systick handler interrupt.
 * It has a default implementation that do anything.
 *
 * @warning The function used to perform operations on each Systick in the system. It
 * be as short as possible because it is called in the Systick interrupt handler.
 *
 * @warning The function shouldn't call an OS API in any case because a new scheduler
 * could occur.
 */
void OS_KERNEL_SysTickHook(void);

/**
 * @brief Function used when happens an error on the OS.
 *
 * @param[in]   caller  Function pointer where error happened.
 */
void OS_KERNEL_ErrorHook(void *caller);

/**
 * @brief Idle task of the operation system.
 */
void OS_KERNEL_IdleTask(void);

/**
 * @brief Method to make an async change of context.
 */
void OS_KERNEL_PortYield(void);

#endif // __OS_KERNEL_H_
