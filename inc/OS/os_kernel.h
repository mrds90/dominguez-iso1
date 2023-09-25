#ifndef __OS_KERNEL_H_
#define __OS_KERNEL_H_

#include <stdint.h>
#include <stdbool.h>
#include "OS/os_config.h"

#define MAX_TASK_SIZE          (MAX_STACK_SIZE / sizeof(uint32_t))  // Defines maximum stack size for a task.
#define SIZE_STACK_FRAME        17U                                 // Size stack frame
#define STACK_POS(x)            (MAX_TASK_SIZE - x)

#define XPSR_VALUE              1 << 24     // xPSR.T = 1
#define EXEC_RETURN_VALUE       0xFFFFFFF9  // EXEC_RETURN value. Return to thread mode with MSP, not use FPU
#define XPSR_REG_POSITION       1U
#define PC_REG_POSTION          2U
#define LR_REG_POSTION          3U
#define R12_REG_POSTION         4U
#define R3_REG_POSTION          5U
#define R2_REG_POSTION          6U
#define R1_REG_POSTION          7U
#define R0_REG_POSTION          8U
#define LR_PREV_VALUE_POSTION   9U
#define R4_REG_POSTION          10U
#define R5_REG_POSTION          11U
#define R6_REG_POSTION          12U
#define R7_REG_POSTION          13U
#define R8_REG_POSTION          14U
#define R9_REG_POSTION          15U
#define R10_REG_POSTION         16U
#define R11_REG_POSTION         17U

/* Possible task status */
typedef uint64_t tick_type_t;

typedef enum {
    OS_TASK_READY,      // Ready state
    OS_TASK_RUNNING,    // Running state
    OS_TASK_BLOCK,      // Blocked state
    OS_TASK_SUSPEND     // Suspended state
} os_task_status_t;

typedef enum
{
    OS_LOW_PRIORITY,
    #if (PRIORITY_LEVELS > 1)
    OS_NORMAL_PRIORITY,
    #if (PRIORITY_LEVELS > 2)
    OS_HIGH_PRIORITY,
    #if (PRIORITY_LEVELS > 3)
    OS_VERYHIGH_PRIORITY,
    #if (PRIORITY_LEVELS > 4)
        #error "Invlid priority level"
    #endif /* (PRIORITY_LEVELS > 4) */
    #endif /* (PRIORITY_LEVELS > 3) */
    #endif /* (PRIORITY_LEVELS > 2) */
    #endif /* (PRIORITY_LEVELS > 1) */

    OS_PRIORITY_QTY,
} os_priority_t;

typedef struct {
    uint32_t memory[MAX_TASK_SIZE];           // Memory stack
    uint32_t stack_pointer;                    // Stack pointer of task
    void *entry_point;                         // Callback executed on task
    uintptr_t id;                             // Task ID, it's a memory position
    os_task_status_t status;                  // Status task.
    os_priority_t priority;                   // Task priority.
    tick_type_t wake_up_time;                 // Time to unblock task if blocked
} os_task_t;


/**
 *@brief Create task.
 *
 *@param[in, out]   handler Data structure of task.
 *@param[in]        priority Task priority level.
 *@param[in]        callback Function executed on task
 *
 *@return Return true if task was success or false in otherwise.
 */
bool OS_KERNEL_TaskCreate(os_task_t *handler, os_priority_t priority, void *callback);

/**
 * @brief Initialization pendSV exception with lowest priority possible.
 */
void OS_KERNEL_Start(void);

/**
 * @brief Get Tick coun of the OS
 *
 * @return tick_type_t
 */
tick_type_t OS_KERNEL_GetTickCount(void);

/**
 * @brief Execute a delay for the current task.
 *
 * @param[in]   tick Number ticks delayed.
 */
void OS_KERNEL_Delay(const uint32_t tick);

/**
 * @brief Function used as default when some task return for a problem.
 */
void OS_KERNEL_ReturnTaskHook(void);

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
void OS_KERNEL_SysTickHook(void);

/**
 * @brief Function used when happen error on OS
 *
 * @param[in]   caller  Function pointer where error happened.
 */
void OS_KERNEL_ErrorHook(void *caller);

/**
 * @brief Idle task of the operation system.
 */
void OS_KERNEL_IdleTask(void);


#endif // __OS_KERNEL_H_
