/**
 * @file keys.c
 * @author Marcos Dominguez
 * @brief GPIO Interrupt HAL for lpc4377
 * @version 0.1
 * @date 2023-10-13
 */

/*==================[ Inclusions ]============================================*/
#include "chip.h"
#include "io_port_lpc4337_edu_ciia.h"


/*=====[ Definitions of private data types ]===================================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Prototypes (declarations) of private functions]======================*/
static void KEYS_ISRConfig(void);

/*=====[Definitions of private global variables]=============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[prototype of private functions]=================================*/

/*=====[Implementations of public functions]=================================*/

/**
 *  @brief Driver Init
 */
void KEYS_Init(void) {
    KEYS_ISRConfig();
}

/*=====[Implementations of private functions]================================*/

/*=====[Implementations of private functions]=================================*/

/**
 * @brief   Inicializa las interrupciones asociadas al driver keys.c
 *          Se realiza sobre las cuatro teclas de la EDUCIAA
 */
void KEYS_ISRConfig(void) {
    //Inicializamos las interrupciones (LPCopen)
    Chip_PININT_Init(LPC_GPIO_PIN_INT);

    //Inicializamos de cada evento de interrupcion (LPCopen)

    /* Help:
    GLOBAL! extern pinInitGpioLpc4337_t gpioPinsInit[];
    Chip_SCU_GPIOIntPinSel( j,  gpioPinsInit[i].gpio.port, gpioPinsInit[i].gpio.pin );   // TECi
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( j ) );                      // INTj (canal j -> hanlder GPIOj)       //Borra el pending de la IRQ
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( j ) );                      // INTj //Selecciona activo por flanco
    Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( j ) );                        // INTj //Selecciona activo por flanco descendente
    Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( j ) );                       // INTj //Selecciona activo por flanco ascendente
    */


    // TEC1 FALL +  TEC1 RISE
    Chip_SCU_GPIOIntPinSel(0, 0, 4);    //(Canal 0 a 7, Puerto GPIO, Pin GPIO)
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0);   //Se configura el canal para que se active por flanco
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0);     //Se configura para que el flanco sea el de bajada
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH0);    //En este caso el flanco es de subida

    // TEC2 FALL + TEC2 RISE
    Chip_SCU_GPIOIntPinSel(1, 0, 8);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH1);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH1);
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH1);


    // TEC3 FALL + TEC3 RISE
    Chip_SCU_GPIOIntPinSel(2, 0, 9);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH2);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH2);
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH2);

    // TEC4 FALL + TEC4 RISE
    Chip_SCU_GPIOIntPinSel(3, 1, 9);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH3);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH3);
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH3);

    NVIC_SetPriority(PIN_INT0_IRQn, 5);
    NVIC_SetPriority(PIN_INT1_IRQn, 5);
    NVIC_SetPriority(PIN_INT2_IRQn, 5);
    NVIC_SetPriority(PIN_INT3_IRQn, 5);
}
