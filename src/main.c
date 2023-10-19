/*=============================================================================
 * Author: Marcos Dominguez <mrds0690@gmail.com>
 * Date: 2023/09/12
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "timer_config.h"
#include "main.h"
#include "io_port_lpc4337_edu_ciia.h"
#include "application.h"
#include "osKernel.h"
#include "osSemaphore.h"
#include "osQueue.h"


#include "sapi.h"
/*=====[Definition macros of private constants]==============================*/

#define SEMAPHORE_TEST  1
#define QUEUE_TEST      2
#define TP_FINAL_TEST   3
#define API_TEST        TP_FINAL_TEST
#define TIMER_RATE      2

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of private methods]======================================*/

#if (API_TEST != TP_FINAL_TEST)

void SystemClock_Config(void);

static void task1(void);

static void task2(void);

static void task3(void);

static void TmrCallback(void);

#endif /* (API_TEST != TP_FINAL_TEST) */

/*=====[Definitions of public global variables]==============================*/


/*=====[Definitions of private global variables]=============================*/
#if (API_TEST != TP_FINAL_TEST)
static osTaskObject osTask1;
static osTaskObject osTask2;
static osTaskObject osTask3;
#if (API_TEST == SEMAPHORE_TEST)
static osSemaphoreObject semaphore1;
static osSemaphoreObject semaphore2;
static osSemaphoreObject semaphore3;
#elif (API_TEST == QUEUE_TEST)
static osQueueObject queue1;
static osQueueObject queue2;
#endif
#endif /* (API_TEST != TP_FINAL_TEST) */
/*=====[Main function, program entry point after power on or reset]==========*/


int main(void) {
    // ----- Setup -----------------------------------

    // ----- Repeat for ever -------------------------
    // Initialize all configured peripherals
    boardConfig();
    KEYS_Init();
    
    #if (API_TEST == TP_FINAL_TEST)
    applicationStart();
    #else
    osTaskCreate(&osTask1, 2, task1);
    osTaskCreate(&osTask2, 2, task2);
    osTaskCreate(&osTask3, 1, task3);
    #if (API_TEST == SEMAPHORE_TEST)
    osSemaphoreInit(&semaphore1, 4, 0);
    osSemaphoreInit(&semaphore2, 0, 0);
    osSemaphoreInit(&semaphore3, 0, 0);
    TIMER_CONFIG_Init(TIMER_RATE, TmrCallback);
    #elif (API_TEST == QUEUE_TEST)
    osQueueInit(&queue1,  sizeof(uint32_t));
    osQueueInit(&queue2, sizeof(uint32_t));

    #endif

    osStart();

    while (1) {
        __WFI();
    }

    // YOU NEVER REACH HERE, because this program runs directly or on a
    // microcontroller and is not called by any Operating System, as in the
    // case of a PC program.
    #endif
    return 0;
}

#if (API_TEST == SEMAPHORE_TEST)

static void task1(void) {
    uint32_t i = 0;

    while (1) {
        osSemaphoreTake(&semaphore1);
        osDelay(100);
        gpioToggle(LED1);
        i++;
    }
}

static void task2(void) {
    static uint32_t j = 0;

    while (1) {
        osSemaphoreTake(&semaphore2);
        gpioToggle(LED2);
        j++;
        if ((j % 2) == 0) {
            osSemaphoreGive(&semaphore1);
        }
    }
}

static void task3(void) {
    uint32_t k = 0;

    while (1) {
        osSemaphoreTake(&semaphore3);
        gpioToggle(LED3);
        k++;
        if ((k % 2) == 0) {
            osSemaphoreGive(&semaphore2);
        }
    }
}

#elif (API_TEST == QUEUE_TEST)

static void task1(void) {
    uint32_t i = 0;

    while (1) {
        osDelay(i * 10 + 10);
        gpioToggle(LED1);
        osQueueReceive(&queue2, &i, 0);
    }
}

static void task2(void) {
    static uint32_t j = 0;

    while (1) {
        if (!osQueueReceive(&queue1, &j, 0)) {
            osDelay(1000);
        }
        else {
            if (osQueueSend(&queue2, &j, 1000)) {
                gpioWrite(LED2, true);
            }
            else {
                gpioWrite(LED2, false);
            }
        }
    }
}

static void task3(void) {
    static uint32_t k = 0;

    while (1) {
        k++;
        if (osQueueSend(&queue1, &k, 500)) {
            gpioWrite(LED3, true);
        }
        else {
            gpioWrite(LED3, false);
        }
    }
}

#endif

#if (API_TEST != TP_FINAL_TEST)
static void TmrCallback(void) {
    osSemaphoreGive(&semaphore3);
    osDelay(1000);
}
#endif
