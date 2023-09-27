#ifndef INC_OSKERNEL_H_
#define INC_OSKERNEL_H_

#include <stdint.h>
#include <stdbool.h>
#include "OS/os_kernel.h"


typedef tick_type_t TickType;               ///< hold tick counts.

typedef os_task_status_t osTaskStatusType;  ///< Status of the OS tasks

typedef os_priority_t osPriorityType;       ///< Priority levels

typedef os_task_t osTaskObject;             ///< task Objects that hold information for the kernel

/**
 *@brief Create task.
 *
 *@param[in, out]   handler Data structure of task.
 *@param[in]       priority Task priority level.
 *@param[in]       callback Function executed on task
 *
 *@return Return true if task was success or false in otherwise.
 */
#define osTaskCreate(handler, priority, callback) OS_KERNEL_TaskCreate((osTaskObject *)handler, (os_priority_t) priority, (void *) callback)

/**
 *@brief Delete task.
 *
 *@param[in, out]   handler Data structure of task, if NULL will delete current task.
 *
 */
#define osTaskDelete(handler) OS_KERNEL_TaskDelete((osTaskObject *)handler)

/**
 *@brief Suspend task.
 *
 *@param[in, out]   handler Data structure of task, if NULL will delete current task.
 *
 */
#define osTaskSuspend(handler) OS_KERNEL_TaskSuspend((osTaskObject *)handler);

/**
 *@brief Resume task.
 *
 *@param[in, out]   handler Data structure of task, if NULL will delete current task.
 *
 */
#define osTaskResume(handler) OS_KERNEL_TaskResume((osTaskObject *)handler)
/**
 * @brief Initialization pendSV exception with lowest priority possible.
 */
#define osStart OS_KERNEL_Start

/**
 * @brief Get Tick coun of the OS
 *
 * @return TickType
 */
#define osGetTickCount OS_KERNEL_GetTickCount

/**
 * @brief Execute a delay for the current task.
 *
 * @param[in]   tick Number ticks delayed.
 */
#define osDelay(tick) OS_KERNEL_Delay(tick)

#define osReturnTaskHook OS_KERNEL_ReturnTaskHook

/**
 * @brief Function used if user would like to do something on systick hander interrupt.
 * It has a default implementation that do anything.
 *
 * @warning The function used to perform operations on each Systick in the system. It
 * be as short as possible because it is called in the Systick interrupt handler.
 *
 * @warning The function shouldn't call an OS API in any case because a new scheduler
 * could occur.
 */
#define osSysTickHook OS_KERNEL_SysTickHook

#define osErrorHook OS_KERNEL_ErrorHook

/**
 * @brief Idle task of the operation system.
 */
#define osIdleTask OS_KERNEL_IdleTask


#endif // INC_OSKERNEL_H_
