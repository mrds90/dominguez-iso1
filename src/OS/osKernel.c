#include <stddef.h>

#include "cmsis.h"
#include "chip.h"
#include "OS/osKernel.h"
#include "cmsis_gcc.h"
/* ==================== Define private variables ==================== */
#define FIRST_INDEX_TASK_PRIORITY     0U

typedef struct {
    osTaskObject *listTask[MAX_NUMBER_TASK];                                // Task list.
    osTaskObject *currentTask;                                              // Current handler task running.
    osTaskObject *nextTask;                                                 // Next handler task will be run.
    uint8_t countTask;                                                      // Number of task created.
} osKernelObject;

/* ================== Private variables declaration ================= */
static osKernelObject os_kernel = {.listTask[0 ... (MAX_NUMBER_TASK - 1)] = NULL,
                                   .countTask = 1};
static tick_tipe_t sys_tick = 0;

/* ================== Private functions declaration ================= */

static uint32_t ChangeOfContext(uint32_t current_stask_pointer);

static void scheduler(void);

static void IdleTask(void);

/* ================= Public functions implementation ================ */

bool osTaskCreate(osTaskObject *handler, void *callback) {
    bool ret = false;
    if (os_kernel.countTask < MAX_NUMBER_TASK) {
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
        handler->id             = os_kernel.countTask;
        handler->stackPointer   = (uint32_t)(handler->memory + MAX_TASK_SIZE - SIZE_STACK_FRAME);

        // Fill controls OS structure
        os_kernel.listTask[os_kernel.countTask] = handler;
        os_kernel.countTask++;

        // Ask to avoid overflow memory when fill element vector
        if (os_kernel.countTask < MAX_NUMBER_TASK) {
            os_kernel.listTask[os_kernel.countTask] = NULL;
        }
    }

    return ret;
}

void osStart(void) {
    extern uint32_t SystemCoreClock;
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(PendSV_IRQn);
    static osTaskObject idle_task;

    // Create idle_task
    idle_task.memory[STACK_POS(XPSR_REG_POSITION)]   = XPSR_VALUE;
    idle_task.memory[STACK_POS(PC_REG_POSTION)]      = (uint32_t)IdleTask;
    idle_task.memory[STACK_POS(LR_PREV_VALUE_POSTION)] = EXEC_RETURN_VALUE;

    idle_task.entryPoint     = IdleTask;
    idle_task.id             = 0;
    idle_task.stackPointer   = (uint32_t)(idle_task.memory + MAX_TASK_SIZE - SIZE_STACK_FRAME);
    os_kernel.listTask[0]    = &idle_task;

    os_kernel.nextTask = NULL;
    os_kernel.currentTask = os_kernel.listTask[0];

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

tick_tipe_t osGetTickCount(void) {
    return sys_tick;
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
    os_kernel.currentTask->stackPointer  = current_stask_pointer;
    os_kernel.currentTask->status        = OS_TASK_READY;

    // Switch address memory points on current task for next task and change state of task
    os_kernel.currentTask            = os_kernel.nextTask;
    os_kernel.currentTask->status    = OS_TASK_RUNNING;

    return os_kernel.currentTask->stackPointer;
}

/**
 * @brief Get the task that must be run.
 *
 * @return Returns true if a new task to be executed.
 */
static void scheduler(void) {
    uint8_t index = 0;

    // Is the first time that operating system execute? Yes, so I start with Task1

    // find next task crated in the task list.
    for (uint8_t i = (os_kernel.currentTask->id + 1); i < MAX_NUMBER_TASK; i++) {
        if (os_kernel.listTask[i] != NULL) {
            index = i;
            break;
        }
    }

    os_kernel.nextTask = os_kernel.listTask[index];
}

static void IdleTask(void) {
    while (1) {}
}

/* ========== Processor Interruption and Exception Handlers ========= */

void SysTick_Handler(void) {
    sys_tick++;
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
    /**
     * Cuando se ingresa al handler de PendSV lo primero que se ejecuta es un push para
     * guardar los registros R4-R11 y el valor de LR, que en este punto es EXEC_RETURN
     * El push se hace al reves de como se escribe en la instruccion, por lo que LR
     * se guarda en la posicion 9 (luego del stack frame). Como la funcion ChangeOfContext
     * se llama con un branch con link, el valor del LR es modificado guardando la direccion
     * de retorno una vez se complete la ejecucion de la funcion
     * El pasaje de argumentos a getContextoSiguiente se hace como especifica el AAPCS siendo
     * el unico argumento pasado por RO, y el valor de retorno tambien se almacena en R0
     *
     * NOTA: El primer ingreso a este handler (luego del reset) implica que el push se hace sobre el
     * stack inicial, ese stack se pierde porque no hay seguimiento del MSP en el primer ingreso
     */
    __ASM volatile ("push {r4-r11, lr}");
    __ASM volatile ("mrs r0, msp");
    __ASM volatile ("bl %0" :: "i" (ChangeOfContext));
    __ASM volatile ("msr msp, r0");
    __ASM volatile ("pop {r4-r11, lr}");    //Recuperados todos los valores de registros

    /* Se hace un branch indirect con el valor de LR que es nuevamente EXEC_RETURN */
    __ASM volatile ("bx lr");
}
