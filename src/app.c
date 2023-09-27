/*=============================================================================
 * Author: Marcos Dominguez <mrds0690@gmail.com>
 * Date: 2023/09/12
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "app.h"
#include "OSAL/osKernel.h"

#include "sapi.h"
/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of private methods]======================================*/

void SystemClock_Config(void);

static void task1(void);

static void task2(void);

static void task3(void);

/*=====[Definitions of public global variables]==============================*/


/*=====[Definitions of private global variables]=============================*/

static osTaskObject osTask1;
static osTaskObject osTask2;
static osTaskObject osTask3;

/*=====[Main function, program entry point after power on or reset]==========*/


int main(void) {
    // ----- Setup -----------------------------------

    // ----- Repeat for ever -------------------------
    // Initialize all configured peripherals
    boardConfig();
    osTaskCreate(&osTask1, 2, task1);
    osTaskCreate(&osTask2, 2, task2);
    osTaskCreate(&osTask3, 1, task3);


    osStart();

    while (1) {
        __WFI();
    }

    // YOU NEVER REACH HERE, because this program runs directly or on a
    // microcontroller and is not called by any Operating System, as in the
    // case of a PC program.
    return 0;
}

static void task1(void) {
    uint32_t i = 0;

    while (1) {
        osDelay(2000);
        gpioToggle(LED1);
        i++;
        if (i == 4) {
            osTaskSuspend(&osTask3);
        }
        if (i == 8) {
            osTaskResume(&osTask3);
            osTaskCreate(&osTask2, 2, task2);
        }
    }
}

static void task2(void) {
    static uint32_t j = 0;

    while (1) {
        osDelay(1000);
        gpioToggle(LED2);
        j++;
        if (j == 4) {
            osTaskDelete(NULL);
        }
    }
}

static void task3(void) {
    uint32_t k = 0;

    while (1) {
        osDelay(500);
        gpioToggle(LED3);
        k++;
    }
}
