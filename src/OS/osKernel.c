#include <stddef.h>


#include "OS/osArchHeaders.h"
#include "OS/osKernel.h"
#include "OS/delay.h"
/* ==================== Define private variables ==================== */
#define FIRST_INDEX_TASK_PRIORITY     0U
#define IDLE_TASK_INDEX               0U
#define TASK_IDLE_PRIORITY            0U

/**
 * @brief Hold the pointers of the fifos for each priority
 *
 */
typedef struct {
    osTaskObject **pop_ptr;
    osTaskObject **push_ptr;
} fifo_task_t;

/**
 * @brief Hold the information of the kernel
 *
 */
typedef struct {
    osTaskObject *listTask[MAX_NUMBER_TASK];                                ///< Task list.
    osTaskObject *currentTask;                                              ///< Current handler task running.
    osTaskObject **nextTask;                                                ///< Next handler task will be run.
    fifo_task_t task_fifo[OS_PRIORITY_QTY];                                 ///< Pointer to fifos with task by priority
    tick_type_t sys_tick;                                                   ///< Tick count of the system
} osKernelObject;


/* ================== Private variables declaration ================= */

static osTaskObject *fifo_task[OS_PRIORITY_QTY][MAX_NUMBER_TASK];          ///< fifos that hold the task by priority
static osKernelObject os_kernel = {.listTask[0 ... (MAX_NUMBER_TASK - 1)] = NULL,
                                   .currentTask = NULL,
                                   .sys_tick = 0,
                                   .task_fifo = {
                                       [OS_LOW_PRIORITY] = {
                                           .pop_ptr = fifo_task[OS_LOW_PRIORITY],
                                           .push_ptr = fifo_task[OS_LOW_PRIORITY],
                                       },
                                    #if (PRIORITY_LEVELS > 1)
                                       [OS_NORMAL_PRIORITY] = {
                                           .pop_ptr = fifo_task[OS_NORMAL_PRIORITY],
                                           .push_ptr = fifo_task[OS_NORMAL_PRIORITY],
                                       },
                                    #if (PRIORITY_LEVELS > 2)
                                       [OS_HIGH_PRIORITY] = {
                                           .pop_ptr = fifo_task[OS_HIGH_PRIORITY],
                                           .push_ptr = fifo_task[OS_HIGH_PRIORITY],
                                       },
                                    #if (PRIORITY_LEVELS > 3)
                                       [OS_VERYHIGH_PRIORITY] = {
                                           .pop_ptr = fifo_task[OS_VERYHIGH_PRIORITY],
                                           .push_ptr = fifo_task[OS_VERYHIGH_PRIORITY],
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
 * @param current_stask_pointer
 * @return uint32_t
 */
static uint32_t ChangeOfContext(uint32_t current_stask_pointer);

/**
 * @brief Choose the next task to be running
 *
 */
static void scheduler(void);

__STATIC_FORCEINLINE void PushTaskToWaitingList(osTaskObject *task);

/* ================= Public functions implementation ================ */
bool osTaskCreate(osTaskObject *handler, osPriorityType priority, void *callback) {
    bool ret = false;
    uint8_t i;
    for (i = (IDLE_TASK_INDEX + 1); i < MAX_NUMBER_TASK; i++) { //find available mem region
        if (os_kernel.listTask[i] == NULL) {
            handler->id = (uintptr_t)&os_kernel.listTask[i]; //save the mem position of the element in the list task as ID
            break;
        }
    }
    if (i < MAX_NUMBER_TASK) { // check if there was memory available
        ret = true;
        // xPSR value with 24 bit on one (Thumb mode).
        handler->memory[STACK_POS(XPSR_REG_POSITION)]   = XPSR_VALUE;
        // Program pointer (PC) points to function used by the task.
        handler->memory[STACK_POS(PC_REG_POSTION)]      = (uint32_t)callback;

        /*
         * Previous Link register (LR) value because handler pendSV call function inside exception
         * and LR is overwrite with the return value of ChangeOfContext.
         */
        handler->memory[STACK_POS(LR_PREV_VALUE_POSTION)] = EXEC_RETURN_VALUE;

        // Pointer function of task.
        handler->entryPoint     = callback;
        handler->stackPointer   = (uint32_t)(handler->memory + MAX_TASK_SIZE - SIZE_STACK_FRAME);
        handler->status         = OS_TASK_READY;
        handler->priority       = priority;


        // Fill controls OS structure
        osTaskObject **temp = (osTaskObject **)handler->id;
        *temp = (handler); //save the handler in the os_kernel.listTask[i].
        PushTaskToWaitingList(handler);
    }

    return ret;
}

void osStart(void) {
    extern uint32_t SystemCoreClock;
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(PendSV_IRQn);
    static osTaskObject idle_task;

    // Create idle_task
    idle_task.memory[STACK_POS(XPSR_REG_POSITION)]      = XPSR_VALUE;
    idle_task.memory[STACK_POS(PC_REG_POSTION)]         = (uint32_t)osIdleTask;
    idle_task.memory[STACK_POS(LR_PREV_VALUE_POSTION)]  = EXEC_RETURN_VALUE;
    idle_task.entryPoint                                = osIdleTask;
    idle_task.id                                        = (uintptr_t) &os_kernel.listTask[IDLE_TASK_INDEX];
    idle_task.status                                    = OS_TASK_READY;
    idle_task.priority                                  = TASK_IDLE_PRIORITY;
    idle_task.stackPointer                              = (uint32_t)(idle_task.memory + MAX_TASK_SIZE - SIZE_STACK_FRAME);
    os_kernel.listTask[IDLE_TASK_INDEX]                 = &idle_task;
    os_kernel.listTask[IDLE_TASK_INDEX]                 = &idle_task;


    // Start in Idle Task
    os_kernel.nextTask = (osTaskObject **)idle_task.id;
    PushTaskToWaitingList(*os_kernel.nextTask);
    /*
     * All interrupts has priority 0 (maximum) at start execution. For that don't happen fault
     * condition, we have to less priotity of NVIC. This math calculation showing take lowest
     * priority possible.
     */
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    /* Activate and configure the time of Systick exception */
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / (1000U * SYSTICK_PERIOD_MS));

    NVIC_EnableIRQ(PendSV_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
}

tick_type_t osGetTickCount(void) {
    return os_kernel.sys_tick;
}

void osDelay(const uint32_t tick) {
    NVIC_DisableIRQ(SysTick_IRQn);

    DELAY_SetDelay(tick, os_kernel.currentTask);

    scheduler();
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
    NVIC_EnableIRQ(SysTick_IRQn);
}

__attribute__((weak)) void osReturnTaskHook(void) {
    while (1) {
        __WFI();
    }
}

__attribute__((weak)) void osSysTickHook(void) {
    __ASM volatile ("nop");
}

__attribute__((weak)) void osErrorHook(void *caller) {
    while (1) {}
}

__attribute__((weak)) void osIdleTask(void) {
    while (1) {
        __WFI();
    }
}

/* ================ Private functions implementation ================ */

/**
 * @brief Get next context task.
 *
 * @param[in] current_stask_pointer Stack pointer of current task.
 *
 * @return Return stack pointer of new task to execute.
 */
static uint32_t ChangeOfContext(uint32_t current_stask_pointer) {
    // Storage last stack pointer used on current task and change state to ready.
    if (os_kernel.currentTask != NULL) {
        os_kernel.currentTask->stackPointer  = current_stask_pointer;
    }

    // Switch address memory points on current task for next task and change state of task
    os_kernel.currentTask            = *os_kernel.nextTask;
    os_kernel.currentTask->status    = OS_TASK_RUNNING;

    return os_kernel.currentTask->stackPointer;
}

/**
 * @brief Get the task that must be run.
 *
 * @return Returns true if a new task to be executed.
 */
static void scheduler(void) {
    for (uint8_t i = 0; i < MAX_NUMBER_TASK; i++) {
        if (os_kernel.listTask[i]->status == OS_TASK_BLOCK) {
            DELAY_EvalDelay(os_kernel.listTask[i]);
            if (os_kernel.listTask[i]->status == OS_TASK_READY) {
                PushTaskToWaitingList(os_kernel.listTask[i]);
            }
        }
    }
    for (uint8_t i = (PRIORITY_LEVELS - 1); i < PRIORITY_LEVELS; i--) {
        if (os_kernel.task_fifo[i].pop_ptr != os_kernel.task_fifo[i].push_ptr) {
            os_kernel.nextTask = os_kernel.task_fifo[i].pop_ptr++;
            if (os_kernel.task_fifo[i].pop_ptr == &fifo_task[i][MAX_NUMBER_TASK]) {
                os_kernel.task_fifo[i].pop_ptr = &fifo_task[i][0];
            }
            break;
        }
    }
    if (os_kernel.currentTask != NULL) {
        if (os_kernel.currentTask != *os_kernel.nextTask) {
            if (os_kernel.currentTask->status == OS_TASK_RUNNING) {
                os_kernel.currentTask->status = OS_TASK_READY;
                PushTaskToWaitingList(os_kernel.currentTask);
            }
        }
    }
}

__STATIC_FORCEINLINE void PushTaskToWaitingList(osTaskObject *task) {
    *os_kernel.task_fifo[task->priority].push_ptr = task;
    os_kernel.task_fifo[task->priority].push_ptr++;
    if (os_kernel.task_fifo[task->priority].push_ptr == &fifo_task[task->priority][MAX_NUMBER_TASK]) {
        os_kernel.task_fifo[task->priority].push_ptr = &fifo_task[task->priority][0];
    }
}

/* ========== Processor Interruption and Exception Handlers ========= */

void SysTick_Handler(void) {
    os_kernel.sys_tick++;
    scheduler();

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
