/***/

#include "osIRQ.h"
#include "timer_config.h"
#include "cmsis_gcc.h"
#include "chip.h"
#include "timer_18xx_43xx.h"
/*================== [macros and definitions] =================================*/
#define TMR0  (0)
#define TMR1  (1)
#define TMR2  (2)
#define TMR3  (3)

#define TIMER_USED TMR0

#define LPC4337_MAX_FREC                204000000 /* Microcontroller frequency */
#define MAX_SYSCALL_INTERRUPT_PRIORITY  5
#define US_TO_TICKS(uS)                 (uS   * (LPC4337_MAX_FREC / 1000000))
#define TICKS_TO_US(ticks)              (ticks  * 1000000 / (LPC4337_MAX_FREC))
#define US_0_1_TO_TICKS(uS01)           (uS01 * (LPC4337_MAX_FREC / 10000000))

/*=====[Definitions of private data type] ==============================*/
typedef void (*callBackFuncPtr_t)(void *);

typedef struct {
    LPC_TIMER_T *name;
    uint32_t RGU; /* Reset Generator Unit */
    uint32_t IRQn;
} timerStaticData_t;

typedef struct {
    callBackFuncPtr_t timerCompareMatchFunctionPointer[4];
} timerDinamicData_t;

typedef enum {
    TIMERCOMPAREMATCH0,
    TIMERCOMPAREMATCH1,
    TIMERCOMPAREMATCH2,
    TIMERCOMPAREMATCH3
} timerCompareMatch_t;

/*=====[Implementation of private methods] ===================================*/

/* Causes:
 * User forgot to initialize the functions for the compare match interrupt on Timer_init call
 */
static void errorOcurred(void *ptr) {
    while (1);
}

static void doNothing(void *ptr) {}

void TIMER_IRQHandler(void *not_used);

/*=====[Definitions of extern global variables] ==============================*/

/*=====[Definitions of public global variables] ==============================*/

static const timerStaticData_t timer_sd[4] = {
    {LPC_TIMER0, RGU_TIMER0_RST, TIMER0_IRQn},
#ifdef CORE_M4
    {LPC_TIMER1, RGU_TIMER1_RST, TIMER1_IRQn},
    {LPC_TIMER2, RGU_TIMER2_RST, TIMER2_IRQn},
#endif
    {LPC_TIMER3, RGU_TIMER3_RST, TIMER3_IRQn}
};

static timerDinamicData_t timer_dd[4] = {
    doNothing, errorOcurred, errorOcurred, errorOcurred,
    doNothing, errorOcurred, errorOcurred, errorOcurred,
    doNothing, errorOcurred, errorOcurred, errorOcurred,
    doNothing, errorOcurred, errorOcurred, errorOcurred
};


/*=====[Main function, program entry point after power on or reset] ==========*/

/*=====[Implementation of private interrupt functions] =======================*/


/*=====[Prototypes (declarations) of private functions] =======================*/

void Chip_TIMER_Reset(LPC_TIMER_T *pTMR);

/*=====[Implementations of public functions] ==================================*/

void TIMER_CONFIG_Init(uint32_t frequency, void *callback) {
    if (frequency == 0) {
        errorOcurred(0);
    }
    uint32_t ticks = US_0_1_TO_TICKS(10000000 / frequency);

    /* Enable timer clock and reset it */
    Chip_TIMER_Init(timer_sd[TIMER_USED].name);
    Chip_RGU_TriggerReset(timer_sd[TIMER_USED].RGU);
    while (Chip_RGU_InReset(timer_sd[TIMER_USED].RGU)) {}
    Chip_TIMER_Reset(timer_sd[TIMER_USED].name);

    /* Update the defalut function pointer name of the Compare match 0*/
    if (callback != NULL) {
        timer_dd[TIMER_USED].timerCompareMatchFunctionPointer[TIMERCOMPAREMATCH0] = callback;
        osRegisterIRQ(TIMER0_IRQn, TIMER_IRQHandler, 10);
    }

    /* Initialize compare match with the specified ticks (number of counts needed to clear the match counter) */
    Chip_TIMER_MatchEnableInt(timer_sd[TIMER_USED].name, TIMERCOMPAREMATCH0);
    Chip_TIMER_SetMatch(timer_sd[TIMER_USED].name, TIMERCOMPAREMATCH0, ticks);

    /* Makes Timer Match 0 period the timer period*/
    Chip_TIMER_ResetOnMatchEnable(timer_sd[TIMER_USED].name, TIMERCOMPAREMATCH0);

    /*Enable timer*/
    Chip_TIMER_Enable(timer_sd[TIMER_USED].name);

    /* Enable timer interrupt */
    NVIC_SetPriority(timer_sd[TIMER_USED].IRQn, MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(timer_sd[TIMER_USED].IRQn);
    NVIC_ClearPendingIRQ(timer_sd[TIMER_USED].IRQn);
}

uint32_t TIMER_CONFIG_GetElapsedTimeUs(void) {
    uint32_t ticks = Chip_TIMER_ReadCount(timer_sd[TIMER_USED].name);
    uint32_t time =  TICKS_TO_US(ticks);
    return time;
}

void TIMER_CONFIG_Reset(void) {
    Chip_TIMER_Reset(timer_sd[TIMER_USED].name);
}

void TIMER_IRQHandler(void *not_used) {
    uint8_t compareMatchNumber = 0;

    for (compareMatchNumber = TIMERCOMPAREMATCH0;
         compareMatchNumber <= TIMERCOMPAREMATCH3;
         compareMatchNumber++) {
        if (Chip_TIMER_MatchPending(timer_sd[TIMER_USED].name, compareMatchNumber)) {
            /*Run the functions saved in the timer dynamic data structure*/
            (*timer_dd[TIMER_USED].timerCompareMatchFunctionPointer[compareMatchNumber])(0);
            Chip_TIMER_ClearMatch(timer_sd[TIMER_USED].name, compareMatchNumber);
        }
    }
}
