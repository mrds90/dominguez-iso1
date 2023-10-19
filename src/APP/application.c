#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "application.h"
#include "GPIOWrapper.h"
#include "SerialWrapper.h"

#include "osKernel.h"
#include "osSemaphore.h"
#include "osQueue.h"
#include "osIRQ.h"

/*==================[macros and definitions]=================================*/

//NOTE: Change at will, only as a placeholder
#define GPIO_BUTTON_1       B1_Pin
#define PORT_BUTTON_1       B1_GPIO_Port
#define EXIT_BUTTON_1       B1_EXTI_IRQn

#define GPIO_BUTTON_2       B2_Pin
#define PORT_BUTTON_2       B2_GPIO_Port
#define EXIT_BUTTON_2       B2_EXTI_IRQn

#define GPIO_LED_RED        LD3_Pin
#define PORT_LED_RED        LD3_GPIO_Port

#define GPIO_LED_GREEN      LD1_Pin
#define PORT_LED_GREEN      LD1_GPIO_Port

#define GPIO_LED_BLUE       LD2_Pin
#define PORT_LED_BLUE       LD2_GPIO_Port

#define GPIO_LED_YELLOW     LD4_Pin
#define PORT_LED_YELLOW     LD4_GPIO_Port

#define GPIO_LED_HEARBEAT   Heartbeat_Pin
#define PORT_LED_HEARBEAT   Heartbeat_GPIO_Port


osTaskObject taskCondition;
osTaskObject taskLedGreen;
osTaskObject taskLedRed;
osTaskObject taskLedYellow;
osTaskObject taskLedBlue;
osTaskObject taskHeartbeat;

osSemaphoreObject semaphoreLed;
osQueueObject queueRed, queueGreen, queueBlue, queueYellow;

/*==================[internal data definition]===============================*/
typedef struct {
    uint64_t tickFallingButton1;    // Se modifica en el flanco descendente del boton 1.
    uint64_t tickRisingButton1;     // Se modifica en el flanco ascendente del boton 1.
    uint64_t tickFallingButton2;    // Se modifica en el flanco descendente del boton 2.
    uint64_t tickRisingButton2;     // Se modifica en el flanco ascendente del boton 2.
}dataTest;

/*==================[external data definition]===============================*/
volatile dataTest times = {0};     // Si bien todas las tareas tiene acceso a la variable times se utilizara la API del OS para validar su funcionamiento.
uint64_t systick = 0;

/*==================[internal functions definition]==========================*/
static char* itoa(int value, char* result, int base);
static void buildString2Send(const uint64_t *delay, const uint64_t *time1, const uint64_t *time2);
static void taskEvaluateCondition(void);
static void taskGreen(void);
static void taskRed(void);
static void taskYellow(void);
static void taskBlue(void);
static void taskLedHearbeat(void);
static void teclasCallback(void *data);


/* =================== [Public functions implementation] =================== */
int applicationStart(void)
{
    //NOTE: At this point, the HW has to be initialized, please do this in the main.c file
    //		Only to simplify the process as STMCube generates autocode inside main

    if (!osTaskCreate(&taskCondition, OS_HIGH_PRIORITY, taskEvaluateCondition))
    {
        while(1)
        {
            __WFI();
        }
    }

    if (!osTaskCreate(&taskLedGreen, OS_NORMAL_PRIORITY, taskGreen))
    {
        while(1)
        {
            __WFI();
        }
    }

    if (!osTaskCreate(&taskLedRed, OS_NORMAL_PRIORITY, taskRed))
    {
        while(1)
        {
            __WFI();
        }
    }

    if (!osTaskCreate(&taskLedYellow, OS_NORMAL_PRIORITY, taskYellow))
    {
        while(1)
        {
            __WFI();
        }
    }

    if (!osTaskCreate(&taskLedBlue, OS_NORMAL_PRIORITY, taskBlue))
    {
        while(1)
        {
            __WFI();
        }
    }

    if (!osTaskCreate(&taskHeartbeat, OS_LOW_PRIORITY, taskLedHearbeat))
    {
        while(1)
        {
            __WFI();
        }
    }

    // Semaphore is created and blocked
    osSemaphoreInit(&semaphoreLed, 1, 1);

    if (!osQueueInit(&queueGreen, sizeof(uint64_t)))
    {
        while(1)
        {
            __WFI();
        }
    }

    if (!osQueueInit(&queueRed, sizeof(uint64_t)))
    {
        while(1)
        {
            __WFI();
        }
    }

    if (!osQueueInit(&queueYellow, sizeof(uint64_t)))
    {
        while(1)
        {
            __WFI();
        }
    }

    if (!osQueueInit(&queueBlue, sizeof(uint64_t)))
    {
        while(1)
        {
            __WFI();
        }
    }

    // TODO: You are able to change this section to suit your current HW
    osRegisterIRQ(EXIT_BUTTON_1, teclasCallback, (void *)&times);
    osRegisterIRQ(EXIT_BUTTON_2, teclasCallback, (void *)&times);

    osStart();

    while (1)
    {
        __WFI();
    }
}



/* ================== [Private functions implementation] =================== */
static char* itoa(int value, char* result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36)
    {
       *result = '\0'; return result;
    }

    char* ptr = result, *ptr1 = result, tmpChar;
    int tmpValue;

    do
    {
        tmpValue = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmpValue - value * base)];
    }while(value);

    // Apply negative sign
    if (tmpValue < 0)
    {
       *ptr++ = '-';
    }

    *ptr-- = '\0';

    while(ptr1 < ptr)
    {
        tmpChar = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmpChar;
    }

    return result;
}

/* Build the text to be send by serial */
static void buildString2Send(const uint64_t *delay, const uint64_t *time1, const uint64_t *time2)
{
    uint8_t t1[21] = {0}, t2[21]= {0}, totalTime[21] = {0};

    itoa(*delay, (char *)totalTime, 10);
    itoa(*time1, (char *)t1, 10);
    itoa(*time2, (char *)t2, 10);

    serialPrint("\t Tiempo encendido: ");
    serialPrint((char *)totalTime);
    serialPrint(" ms\n\r");


    serialPrint("\t Tiempo entre flancos descendentes: ");
    serialPrint((char *)t1);
    serialPrint(" ms\n\r");


    serialPrint("\t Tiempo entre flancos ascendentes: ");
    serialPrint((char *)t2);
    serialPrint(" ms\n\r");
}


static void taskEvaluateCondition(void)
{
    uint64_t delay = 0, time1 = 0, time2 = 0;

    while (1)
    {
        osSemaphoreTake(&semaphoreLed);

        if (times.tickFallingButton1 == 0 && times.tickRisingButton1 == 0 && times.tickFallingButton2 == 0 && times.tickRisingButton2 == 0)
        {
            continue;
        }

        // In this case, the green or red led turn on.
        if (times.tickFallingButton1 < times.tickFallingButton2)
        {
            // Avoid use case when there isn't intersection in the buttons.
            if (times.tickRisingButton1 < times.tickFallingButton2)
            {
                memset((void *)&times, 0, sizeof(dataTest));
                continue;
            }

            if (times.tickRisingButton1 < times.tickRisingButton2)
            {
                // Turn on green led
                time1 = (times.tickFallingButton2 - times.tickFallingButton1);
                time2 = (times.tickRisingButton2 - times.tickRisingButton1);
                delay = time1 + time2;
                serialPrint("Led Verde encendido:\n\r");
                buildString2Send(&delay, &time1, &time2);
                osQueueSend(&queueGreen, &delay, OS_MAX_DELAY);
            }
            else
            {
                // Turn on red led
                time1 = (times.tickFallingButton2 - times.tickFallingButton1);
                time2 = (times.tickRisingButton1 - times.tickRisingButton2);
                delay = time1 + time2;
                serialPrint("Led Rojo encendido:\n\r");
                buildString2Send(&delay, &time1, &time2);
                osQueueSend(&queueRed, &delay, OS_MAX_DELAY);
            }
        }
        // In this case, the yellow or blue led turn on.
        else
        {
            // Avoid use case when there isn't intersection in the buttons.
            if (times.tickRisingButton2 < times.tickFallingButton1)
            {
                memset((void *)&times, 0, sizeof(dataTest));
                continue;
            }

            if (times.tickRisingButton1 < times.tickRisingButton2)
            {
                // Turn on yellow led
                time1 = (times.tickFallingButton1 - times.tickFallingButton2);
                time2 = (times.tickRisingButton2 - times.tickRisingButton1);
                delay = time1 + time2;
                serialPrint("Led Amarillo encendido:\n\r");
                buildString2Send(&delay, &time1, &time2);
                osQueueSend(&queueYellow, &delay, OS_MAX_DELAY);
            }
            else
            {
                // Turn on blue led
                time1 = (times.tickFallingButton1 - times.tickFallingButton2);
                time2 = (times.tickRisingButton1 - times.tickRisingButton2);
                delay = time1 + time2;
                serialPrint("Led azul encendido:\n\r");
                buildString2Send(&delay, &time1, &time2);
                osQueueSend(&queueBlue, &delay, OS_MAX_DELAY);
            }
        }

        memset((void *)&times, 0, sizeof(dataTest));
    }

}

/**
 * @brief Drivers the green led.
 */
static void taskGreen(void)
{
    uint64_t delay = 0;

    while(1)
    {
        osQueueReceive(&queueGreen, &delay, OS_MAX_DELAY);
        gpioSetLevel(GPIO_LED_GREEN, (uint32_t)PORT_LED_GREEN, true);
        osDelay((uint32_t)delay);
        gpioSetLevel(GPIO_LED_GREEN, (uint32_t)PORT_LED_GREEN, false);
        delay = 0;
    }
}

/**
 * @brief Drivers the red led.
 */
static void taskRed(void)
{
    uint64_t delay = 0;

    while(1)
    {
        osQueueReceive(&queueRed, &delay, OS_MAX_DELAY);
        gpioSetLevel(GPIO_LED_RED, (uint32_t)PORT_LED_RED, true);
        osDelay((uint32_t)delay);
        gpioSetLevel(GPIO_LED_RED, (uint32_t)PORT_LED_RED, false);
        delay = 0;
    }
}

/**
 * @brief Drivers the yellow led.
 */
static void taskYellow(void)
{
    uint64_t delay = 0;

    while(1)
    {
        osQueueReceive(&queueYellow, &delay, OS_MAX_DELAY);
        gpioSetLevel(GPIO_LED_YELLOW, (uint32_t)PORT_LED_YELLOW, true);
        osDelay((uint32_t)delay);
        gpioSetLevel(GPIO_LED_YELLOW, (uint32_t)PORT_LED_YELLOW, false);
        delay = 0;
    }
}

/**
 * @brief Drivers the blue led.
 */
static void taskBlue(void)
{
    uint64_t delay = 0;

    while(1)
    {
        osQueueReceive(&queueBlue, &delay, OS_MAX_DELAY);
        gpioSetLevel(GPIO_LED_BLUE, (uint32_t)PORT_LED_BLUE, true);
        osDelay((uint32_t)delay);
        gpioSetLevel(GPIO_LED_BLUE, (uint32_t)PORT_LED_BLUE, false);
        delay = 0;
    }
}

/**
 * @brief Drivers the heartbeat led.
 */
static void taskLedHearbeat(void)
{
    while(1)
    {
        osDelay(1000);
        gpioSetLevel(GPIO_LED_HEARBEAT, (uint32_t)PORT_LED_HEARBEAT, true);
        osDelay(1000);
        gpioSetLevel(GPIO_LED_HEARBEAT, (uint32_t)PORT_LED_HEARBEAT, false);
    }
}

void osSysTickHook(void)
{
    systick++;
}

/**
 * @brief Detects the Falling/Rising edges of buttons.
 */
void teclasCallback(void *data)
{
    dataTest *time = (dataTest* )data;

    if(__HAL_GPIO_EXTI_GET_IT(GPIO_BUTTON_1) != RESET)
    {
      __HAL_GPIO_EXTI_CLEAR_IT(GPIO_BUTTON_1);


      if (gpioGetLevel(GPIO_BUTTON_1, (uint32_t)PORT_BUTTON_1))
      {
          // Rising edge detected.
          time->tickRisingButton1 = systick;
      }
      else
      {
          // Falling edge detected.
          time->tickFallingButton1 = systick;
          time->tickRisingButton1 = 0;
      }
    }

    if(__HAL_GPIO_EXTI_GET_IT(GPIO_BUTTON_2) != RESET)
    {
      __HAL_GPIO_EXTI_CLEAR_IT(GPIO_BUTTON_2);

      if (gpioGetLevel(GPIO_BUTTON_2, (uint32_t)PORT_BUTTON_2))
      {
          // Rising edge detected.
          time->tickRisingButton2 = systick;
      }
      else
      {
          // Falling edge detected.
          time->tickFallingButton2 = systick;
          time->tickRisingButton2 = 0;
      }
    }

    if (time->tickFallingButton1 != 0 && time->tickRisingButton1 != 0 && time->tickFallingButton2 != 0 && time->tickRisingButton2 != 0)
    {
        osSemaphoreGive(&semaphoreLed);
    }
}
