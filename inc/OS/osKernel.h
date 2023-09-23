#ifndef INC_OSKERNEL_H_
#define INC_OSKERNEL_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_NUMBER_TASK         9U                                  // Defines maximum task we could create.
#define MAX_STACK_SIZE          256U                                // Defines maximum stack size for a task.
#define MAX_TASK_SIZE          (MAX_STACK_SIZE / sizeof(uint32_t))    // Defines maximum stack size for a task.
#define SYSTICK_PERIOD_MS       1U                                  // Systick period time in mili-second.
#define SIZE_STACK_FRAME        17U                                 // Size stack frame
#define STACK_POS(x)            (MAX_TASK_SIZE - x)
#define PRIORITY_LEVELS         4U

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
typedef enum {
    OS_TASK_READY,      // Ready state
    OS_TASK_RUNNING,    // Running state
    OS_TASK_BLOCK,      // Blocked state
    OS_TASK_SUSPEND     // Suspended state
} osTaskStatusType;

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
} osPriorityType;

typedef struct {
    uint32_t memory[MAX_TASK_SIZE];           // Memory stack
    uint32_t stackPointer;                    // Stack pointer of task
    void *entryPoint;                         // Callback executed on task
    uintptr_t id;                             // Task ID, it's a memory position
    osTaskStatusType status;                  // Status task.
    osPriorityType priority;                  // Task priority.
} osTaskObject;

typedef uint64_t tick_tipe_t;

/**
*@brief Create task.
*
*@param[in, out]   handler Data structure of task.
*@param[in]       priority Task priority level.
*@param[in]       callback Function executed on task
*
*@return Return true if task was success or false in otherwise.
*/

bool osTaskCreate(osTaskObject *handler, osPriorityType priority, void *callback);

/**
 * @brief Initialization pendSV exception with lowest priority possible.
 */
void osStart(void);

tick_tipe_t osGetTickCount(void);

/**
 * @brief Execute a delay for the current task.
 *
 * @param[in]   tick Number ticks delayed.
 */
void osDelay(const uint32_t tick);

/**
 * @brief Function used as default when some task return for a problem.
 */
void osReturnTaskHook(void);

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
void osSysTickHook(void);

/**
 * @brief Function used when happen error on OS
 *
 * @param[in]   caller  Function pointer where error happened.
 */
void osErrorHook(void *caller);

/**
 * @brief Idle task of the operation system.
 */
void osIdleTask(void);


#endif // INC_OSKERNEL_H_
