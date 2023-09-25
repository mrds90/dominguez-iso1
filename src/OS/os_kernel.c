#include <stddef.h>


#include "OS/os_arch_headers.h"
#include "OS/os_kernel.h"
#include "OS/delay.h"
/* ==================== Define private variables ==================== */

#define IDLE_TASK_INDEX               0U
#define TASK_IDLE_PRIORITY            0U

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
    fifo_task_t task_fifo[OS_PRIORITY_QTY];                                 ///< Pointer to fifos with task by priority
    tick_type_t sys_tick;                                                   ///< Tick count of the system
} os_kernel_t;


/* ================== Private variables declaration ================= */

static os_task_t *fifo_task[OS_PRIORITY_QTY][MAX_NUMBER_TASK];          ///< fifos that hold the task by priority
static os_kernel_t os_kernel = {.list_task[0 ... (MAX_NUMBER_TASK - 1)] = NULL,
                                   .current_task = NULL,
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
 * @param current_stack_pointer
 * @return uint32_t
 */
static uint32_t ChangeOfContext(uint32_t current_stack_pointer);

/**
 * @brief Choose the next task to be running
 *
 */
static void Scheduler(void);

__STATIC_FORCEINLINE void PushTaskToWaitingList(os_task_t *task);

/* ================= Public functions implementation ================ */
bool OS_KERNEL_TaskCreate(os_task_t *handler, os_priority_t priority, void *callback) {
    bool ret = false;
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

        /*
         * Previous Link register (LR) value because handler pendSV call function inside exception
         * and LR is overwrite with the return value of ChangeOfContext.
         */
        handler->memory[STACK_POS(LR_PREV_VALUE_POSTION)] = EXEC_RETURN_VALUE;

        // Pointer function of task.
        handler->entry_point     = callback;
        handler->stack_pointer   = (uint32_t)(handler->memory + MAX_TASK_SIZE - SIZE_STACK_FRAME);
        handler->status         = OS_TASK_READY;
        handler->priority       = priority;


        // Fill controls OS structure
        os_task_t **temp = (os_task_t **)handler->id;
        *temp = (handler); //save the handler in the os_kernel.list_task[i].
        PushTaskToWaitingList(handler);
    }

    return ret;
}

void OS_KERNEL_Start(void) {
    extern uint32_t SystemCoreClock;
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(PendSV_IRQn);
    static os_task_t idle_task;

    // Create idle_task
    idle_task.memory[STACK_POS(XPSR_REG_POSITION)]      = XPSR_VALUE;
    idle_task.memory[STACK_POS(PC_REG_POSTION)]         = (uint32_t)OS_KERNEL_IdleTask;
    idle_task.memory[STACK_POS(LR_PREV_VALUE_POSTION)]  = EXEC_RETURN_VALUE;
    idle_task.entry_point                                = OS_KERNEL_IdleTask;
    idle_task.id                                        = (uintptr_t) &os_kernel.list_task[IDLE_TASK_INDEX];
    idle_task.status                                    = OS_TASK_READY;
    idle_task.priority                                  = TASK_IDLE_PRIORITY;
    idle_task.stack_pointer                              = (uint32_t)(idle_task.memory + MAX_TASK_SIZE - SIZE_STACK_FRAME);
    os_kernel.list_task[IDLE_TASK_INDEX]                 = &idle_task;
    os_kernel.list_task[IDLE_TASK_INDEX]                 = &idle_task;


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

    NVIC_EnableIRQ(PendSV_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
}

tick_type_t OS_KERNEL_GetTickCount(void) {
    return os_kernel.sys_tick;
}

void OS_KERNEL_Delay(const uint32_t tick) {
    NVIC_DisableIRQ(SysTick_IRQn);

    DELAY_SetDelay(tick, os_kernel.current_task);

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
    NVIC_EnableIRQ(SysTick_IRQn);
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
        if (os_kernel.list_task[i]->status == OS_TASK_BLOCK) {
            DELAY_EvalDelay(os_kernel.list_task[i]);
            if (os_kernel.list_task[i]->status == OS_TASK_READY) {
                PushTaskToWaitingList(os_kernel.list_task[i]);
            }
        }
    }
    for (uint8_t i = (PRIORITY_LEVELS - 1); i < PRIORITY_LEVELS; i--) {
        if (os_kernel.task_fifo[i].pop_ptr != os_kernel.task_fifo[i].push_ptr) {
            os_kernel.next_task = os_kernel.task_fifo[i].pop_ptr++;
            if (os_kernel.task_fifo[i].pop_ptr == &fifo_task[i][MAX_NUMBER_TASK]) {
                os_kernel.task_fifo[i].pop_ptr = &fifo_task[i][0];
            }
            break;
        }
        else if ((i == os_kernel.current_task->priority) && (os_kernel.current_task->status == OS_TASK_RUNNING)) {
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
