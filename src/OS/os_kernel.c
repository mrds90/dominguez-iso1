#include <stddef.h>


#include "os_arch_headers.h"
#include "os_kernel.h"
#include "delay.h"
#include "os_methods.h"
#include "os_irq.h"

#define IDLE_TASK_INDEX         0U
#define TASK_IDLE_PRIORITY      0U

#define SIZE_STACK_FRAME        17U                         // Size stack frame
#define STACK_POS(x)            (MAX_TASK_SIZE - x)
#define XPSR_VALUE              1 << 24                     // xPSR.T = 1
#define EXEC_RETURN_VALUE       0xFFFFFFF9                  // EXEC_RETURN value. Return to thread mode with MSP, not use FPU
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

/* ==================== Define private data type ==================== */
typedef enum {
    OS_STATUS_RESET,
    OS_STATUS_RUNNING,
    OS_STATUS_INTERRUPTED,

    OS_STATUS_QTY,
} os_status_t;

/**
 * @brief Hold the pointers of the fifos for each priority
 *
 */
typedef struct {
    os_task_t **pop_ptr;
    os_task_t **push_ptr;
} fifo_task_t;

/**
 * @brief Hold the information of the kernel
 *
 */
typedef struct {
    os_task_t *list_task[MAX_NUMBER_TASK];                                ///< Task list.
    os_task_t *current_task;                                              ///< Current handler task running.
    os_task_t **next_task;                                                ///< Next handler task will be run.
    fifo_task_t task_fifo[OS_KERNEL_PRIORITY_QTY];                        ///< Pointer to fifos with task by priority
    tick_type_t sys_tick;                                                 ///< Tick count of the system
    os_status_t status;                                                   ///< Reset (Not started) - Running - Interrupted
} os_kernel_t;


/* ================== Private variables declaration ================= */

static os_task_t *fifo_task[OS_KERNEL_PRIORITY_QTY][MAX_NUMBER_TASK];          ///< fifos that hold the task by priority
static os_kernel_t os_kernel = {.list_task[0 ... (MAX_NUMBER_TASK - 1)] = NULL,
                                .current_task = NULL,
                                .sys_tick = 0,
                                .status = OS_STATUS_RESET,
                                .task_fifo = {
                                    [OS_KERNEL_LOW_PRIORITY] = {
                                        .pop_ptr = fifo_task[OS_KERNEL_LOW_PRIORITY],
                                        .push_ptr = fifo_task[OS_KERNEL_LOW_PRIORITY],
                                    },
                                    #if (PRIORITY_LEVELS > 1)
                                    [OS_KERNEL_NORMAL_PRIORITY] = {
                                        .pop_ptr = fifo_task[OS_KERNEL_NORMAL_PRIORITY],
                                        .push_ptr = fifo_task[OS_KERNEL_NORMAL_PRIORITY],
                                    },
                                    #if (PRIORITY_LEVELS > 2)
                                    [OS_KERNEL_HIGH_PRIORITY] = {
                                        .pop_ptr = fifo_task[OS_KERNEL_HIGH_PRIORITY],
                                        .push_ptr = fifo_task[OS_KERNEL_HIGH_PRIORITY],
                                    },
                                    #if (PRIORITY_LEVELS > 3)
                                    [OS_KERNEL_VERYHIGH_PRIORITY] = {
                                        .pop_ptr = fifo_task[OS_KERNEL_VERYHIGH_PRIORITY],
                                        .push_ptr = fifo_task[OS_KERNEL_VERYHIGH_PRIORITY],
                                    },
                                    #if (PRIORITY_LEVELS > 4)
                                        #error "Invlid priority level"
                                    #endif /* (PRIORITY_LEVELS > 4) */
                                    #endif /* (PRIORITY_LEVELS > 3) */
                                    #endif /* (PRIORITY_LEVELS > 2) */
                                    #endif /* (PRIORITY_LEVELS > 1) */
                                }};

/* ================== Private functions declaration ================= */

/**
 * @brief Save the stack pointer of the current task and load the msp with the stack pointer of the next task.
 *
 * @param current_stack_pointer
 * @return uint32_t
 */
static uint32_t ChangeOfContext(uint32_t current_stack_pointer);

/**
 * @brief Choose the next task to be running
 *
 */
static void Scheduler(void);

/**
 * @brief Send to a priority FIFO the task
 *
 * @param task task handler to push
 */
__STATIC_FORCEINLINE void PushTaskToWaitingList(os_task_t *task);

/**
 * @brief Yield method
 *
 */
__STATIC_FORCEINLINE void AsyncChangeOfContext(void);

/* ================= Public functions implementation ================ */

bool OS_KERNEL_TaskCreate(os_task_t *handler, os_priority_t priority, void *callback) {
    bool ret = false;
    if (handler != NULL) {
        if (callback != NULL) {
            uint8_t i;
            for (i = (IDLE_TASK_INDEX + 1); i < MAX_NUMBER_TASK; i++) { //find available mem region
                if (os_kernel.list_task[i] == NULL) {
                    handler->id = (uintptr_t)&os_kernel.list_task[i]; //save the mem position of the element in the list task as ID
                    break;
                }
            }
            if (i < MAX_NUMBER_TASK) { // check if there was memory available
                ret = true;
                // xPSR value with 24 bit on one (Thumb mode).
                handler->memory[STACK_POS(XPSR_REG_POSITION)]   = XPSR_VALUE;
                // Program pointer (PC) points to function used by the task.
                handler->memory[STACK_POS(PC_REG_POSTION)]      = (uint32_t)callback;
                handler->memory[STACK_POS(LR_REG_POSTION)]      = (uint32_t)OS_KERNEL_ReturnTaskHook;

                /*
                 * Previous Link register (LR) value because handler pendSV call function inside exception
                 * and LR is overwrite with the return value of ChangeOfContext.
                 */
                handler->memory[STACK_POS(LR_PREV_VALUE_POSTION)] = EXEC_RETURN_VALUE;

                // Pointer function of task.
                handler->entry_point     = callback;
                handler->stack_pointer   = (uint32_t)(handler->memory + MAX_TASK_SIZE - SIZE_STACK_FRAME);
                handler->status         = OS_TASK_READY;
                if ((uint8_t)(priority) >= PRIORITY_LEVELS) {
                    handler->priority = PRIORITY_LEVELS - 1;
                }
                else {
                    handler->priority       = priority;
                }


                // Fill controls OS structure
                os_task_t **temp = (os_task_t **)handler->id;
                *temp = (handler); //save the handler in the os_kernel.list_task[i].
                PushTaskToWaitingList(handler);
            }
        }
    }

    return ret;
}

void OS_KERNEL_TaskDelete(os_task_t *handler) {
    NVIC_DisableIRQ(SysTick_IRQn);

    bool delete_current_task = false;
    if (handler == NULL) {
        handler = os_kernel.current_task;
        delete_current_task = true;
    }

    os_task_t **temp = (os_task_t **)handler->id;
    *temp = (os_task_t *)(NULL);
    if (delete_current_task) {
        os_kernel.current_task = (os_task_t *)(NULL);
    }
    os_kernel.next_task = &os_kernel.list_task[IDLE_TASK_INDEX];
    AsyncChangeOfContext();
    NVIC_EnableIRQ(SysTick_IRQn);
}

void OS_KERNEL_TaskSuspend(os_task_t *handler) {
    if ((handler == NULL) || (handler->status != OS_TASK_SUSPEND)) {
        NVIC_DisableIRQ(SysTick_IRQn);
        if (handler == NULL) {
            handler = os_kernel.current_task;
            if (handler == NULL) {
                return;
            }
            handler->status = OS_TASK_READY;
        }

        handler->prev_status = handler->status;

        handler->status = OS_TASK_SUSPEND;


        os_kernel.next_task = &os_kernel.list_task[IDLE_TASK_INDEX];
        AsyncChangeOfContext();
        NVIC_EnableIRQ(SysTick_IRQn);
    }
}

void OS_KERNEL_TaskResume(os_task_t *handler) {
    if (handler != NULL) {
        if (handler->status == OS_TASK_SUSPEND) {
            NVIC_DisableIRQ(SysTick_IRQn);

            handler->status = handler->prev_status;
            PushTaskToWaitingList(handler);
            if (handler->priority > os_kernel.current_task->priority) {
                if (handler->status == OS_TASK_READY) {
                    AsyncChangeOfContext();
                }
            }
            NVIC_EnableIRQ(SysTick_IRQn);
        }
    }
}

void OS_KERNEL_Start(void) {
    extern uint32_t SystemCoreClock;
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(PendSV_IRQn);
    static os_task_t idle_task;

    // Create idle_task
    idle_task.memory[STACK_POS(XPSR_REG_POSITION)]      = XPSR_VALUE;
    idle_task.memory[STACK_POS(PC_REG_POSTION)]         = (uint32_t)OS_KERNEL_IdleTask;
    idle_task.memory[STACK_POS(LR_REG_POSTION)]         = (uint32_t)OS_KERNEL_ReturnTaskHook;
    idle_task.memory[STACK_POS(LR_PREV_VALUE_POSTION)]  = EXEC_RETURN_VALUE;
    idle_task.entry_point                               = OS_KERNEL_IdleTask;
    idle_task.id                                        = (uintptr_t) &os_kernel.list_task[IDLE_TASK_INDEX];
    idle_task.status                                    = OS_TASK_READY;
    idle_task.priority                                  = TASK_IDLE_PRIORITY;
    idle_task.stack_pointer                             = (uint32_t)(idle_task.memory + MAX_TASK_SIZE - SIZE_STACK_FRAME);
    os_kernel.list_task[IDLE_TASK_INDEX]                = &idle_task;
    os_kernel.list_task[IDLE_TASK_INDEX]                = &idle_task;


    // Start in Idle Task
    os_kernel.next_task = (os_task_t **)idle_task.id;
    PushTaskToWaitingList(*os_kernel.next_task);
    /*
     * All interrupts has priority 0 (maximum) at start execution. For that don't happen fault
     * condition, we have to less priotity of NVIC. This math calculation showing take lowest
     * priority possible.
     */
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    /* Activate and configure the time of Systick exception */
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / (1000U * SYSTICK_PERIOD_MS));

    os_kernel.status = OS_STATUS_RUNNING;

    NVIC_EnableIRQ(PendSV_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
}

tick_type_t OS_KERNEL_GetTickCount(void) {
    return os_kernel.sys_tick;
}

void OS_KERNEL_Delay(const tick_type_t tick) {
    if ((tick > 0) && (os_kernel.status == OS_STATUS_RUNNING)) {
        NVIC_DisableIRQ(SysTick_IRQn);

        DELAY_SetDelay(tick, os_kernel.current_task);

        AsyncChangeOfContext();

        NVIC_EnableIRQ(SysTick_IRQn);
    }
}

__attribute__((weak)) void OS_KERNEL_ReturnTaskHook(void) {
    while (1) {
        __WFI();
    }
}

__attribute__((weak)) void OS_KERNEL_SysTickHook(void) {
    __ASM volatile ("nop");
}

__attribute__((weak)) void OS_KERNEL_ErrorHook(void *caller) {
    while (1) {}
}

__attribute__((weak)) void OS_KERNEL_IdleTask(void) {
    while (1) {
        __WFI();
    }
}

void OS_KERNEL_PortYield(void) {
    AsyncChangeOfContext();
}

/* ================ Private functions implementation ================ */

/**
 * @brief Get next context task.
 *
 * @param[in] current_stack_pointer Stack pointer of current task.
 *
 * @return Return stack pointer of new task to execute.
 */
static uint32_t ChangeOfContext(uint32_t current_stack_pointer) {
    // Storage last stack pointer used on current task and change state to ready.
    if (os_kernel.current_task != NULL) {
        os_kernel.current_task->stack_pointer  = current_stack_pointer;
    }

    // Switch address memory points on current task for next task and change state of task
    os_kernel.current_task            = *os_kernel.next_task;
    os_kernel.current_task->status    = OS_TASK_RUNNING;

    return os_kernel.current_task->stack_pointer;
}

/**
 * @brief Get the task that must be run.
 *
 * @return Returns true if a new task to be executed.
 */
static void Scheduler(void) {
    for (uint8_t i = 0; i < MAX_NUMBER_TASK; i++) {
        if (DELAY_EvalDelay(os_kernel.list_task[i])) {
            os_kernel.list_task[i]->status = OS_TASK_READY;
            PushTaskToWaitingList(os_kernel.list_task[i]);
        }
    }
    for (uint8_t i = (PRIORITY_LEVELS - 1); i < PRIORITY_LEVELS; i--) {
        while (os_kernel.task_fifo[i].pop_ptr != os_kernel.task_fifo[i].push_ptr) {
            os_kernel.next_task = os_kernel.task_fifo[i].pop_ptr++;
            if (os_kernel.task_fifo[i].pop_ptr == &fifo_task[i][MAX_NUMBER_TASK]) {
                os_kernel.task_fifo[i].pop_ptr = &fifo_task[i][0];
            }
            os_task_t *temp = *os_kernel.next_task;
            if (temp->status == OS_TASK_READY) {
                break;
            }
        }
        if ((i == os_kernel.current_task->priority) && (os_kernel.current_task->status == OS_TASK_RUNNING) || (os_kernel.current_task != *os_kernel.next_task)) {
            break;
        }
    }
    if (os_kernel.current_task != NULL) {
        if (os_kernel.current_task != *os_kernel.next_task) {
            if (os_kernel.current_task->status == OS_TASK_RUNNING) {
                os_kernel.current_task->status = OS_TASK_READY;
                PushTaskToWaitingList(os_kernel.current_task);
            }
        }
    }
}

__STATIC_FORCEINLINE void PushTaskToWaitingList(os_task_t *task) {
    *os_kernel.task_fifo[task->priority].push_ptr = task;
    os_kernel.task_fifo[task->priority].push_ptr++;
    if (os_kernel.task_fifo[task->priority].push_ptr == &fifo_task[task->priority][MAX_NUMBER_TASK]) {
        os_kernel.task_fifo[task->priority].push_ptr = &fifo_task[task->priority][0];
    }
}

__STATIC_FORCEINLINE void AsyncChangeOfContext(void) {
    Scheduler();
    /*
     * Set up bit corresponding exception PendSV
     */
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

    /*
     * Instruction Synchronization Barrier; flushes the pipeline and ensures that
     * all previous instructions are completed before executing new instructions
     */
    __ISB();

    /*
     * Data Synchronization Barrier; ensures that all memory accesses are
     * completed before next instruction is executed
     */
    __DSB();
}

void OS_METHODS_SetTaskAsReady(os_task_t *handler) {
    if (handler->status != OS_TASK_READY) {
        handler->status = OS_TASK_READY;
        PushTaskToWaitingList(handler);
    }
}

os_task_t*OS_METHODS_GetCurrentTask(void) {
    return os_kernel.current_task;
}

void OS_METHODS_InterruptState(bool status) {
    static os_status_t status_history[IRQ_NUMBER];
    static os_status_t *status_history_ptr = status_history;
    if (status) {
        *status_history_ptr = os_kernel.status;
        status_history_ptr++;
        os_kernel.status = OS_STATUS_INTERRUPTED;
    }
    else {
        status_history_ptr--;
        os_kernel.status = *status_history_ptr;
    }
}

/* ========== Processor Interruption and Exception Handlers ========= */

void SysTick_Handler(void) {
    os_kernel.sys_tick++;
    Scheduler();
    OS_KERNEL_SysTickHook();
    /*
     * Set up bit corresponding exception PendSV
     */
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

    /*
     * Instruction Synchronization Barrier; flushes the pipeline and ensures that
     * all previous instructions are completed before executing new instructions
     */
    __ISB();

    /*
     * Data Synchronization Barrier; ensures that all memory accesses are
     * completed before next instruction is executed
     */
    __DSB();
}

__attribute__ ((naked)) void PendSV_Handler(void) {
    __ASM volatile ("tst lr, 0x10");
    __ASM volatile ("it eq");
    __ASM volatile ("vpusheq {s16-s31}");

    __ASM volatile ("push {r4-r11, lr}");
    __ASM volatile ("mrs r0, msp");
    __ASM volatile ("bl %0" :: "i" (ChangeOfContext));
    __ASM volatile ("msr msp, r0");
    __ASM volatile ("pop {r4-r11, lr}");     //Recuperados todos los valores de registros

    __ASM volatile ("tst lr,0x10");
    __ASM volatile ("it eq");
    __ASM volatile ("vpopeq {s16-s31}");
    /* Se hace un branch indirect con el valor de LR que es nuevamente EXEC_RETURN */
    __ASM volatile ("bx lr");
}
