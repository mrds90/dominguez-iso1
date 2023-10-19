/**
 * @file keys.c
 * @author Marcos Dominguez
 *
 * @brief GPIO Interrupt HAL for lpc4377
 *
 * @version 0.1
 * @date 2023-10-13
 */

/*================== [ Inclusions ] ============================================*/

#include "chip.h"
#include "io_port_lpc4337_edu_ciaa.h"

/*===== [Definitions of private data types] ===================================*/

/*===== [Definition macros of private constants] ==============================*/

/*===== [Prototypes (declarations) of private functions] ======================*/

/**
 * @brief   Initializes interrupts associated with the keys.c driver.
 *          This is done for the four keys on EDUCIAA.
 */
static void KEYS_ISRConfig(void);

/*===== [Definitions of private global variables] =============================*/

/*===== [Definitions of public global variables] ==============================*/

/*===== [prototype of private functions] ======================================*/

/*===== [Implementations of public functions] =================================*/

void KEYS_Init(void) {
    KEYS_ISRConfig();
}

/*===== [Implementations of private functions] ================================*/

/*===== [Implementations of private functions] ================================*/

void KEYS_ISRConfig(void) {
    // Initialize interrupts (LPCopen)
    Chip_PININT_Init(LPC_GPIO_PIN_INT);

    // Initialize each interrupt event (LPCopen)

    /* Help:
    GLOBAL! extern pinInitGpioLpc4337_t gpioPinsInit[];
    Chip_SCU_GPIOIntPinSel( j,  gpioPinsInit[i].gpio.port, gpioPinsInit[i].gpio.pin );   // TECi
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( j ) );                      // INTj (channel j -> handler GPIOj)   // Clears the pending IRQ
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( j ) );                      // INTj // Selects edge-sensitive mode
    Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( j ) );                        // INTj // Selects low-level trigger
    Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( j ) );                       // INTj // Selects high-level trigger
    */

    // TEC1 FALL +  TEC1 RISE
    Chip_SCU_GPIOIntPinSel(0, 0, 4);                           //(Channel 0 to 7, GPIO Port, GPIO Pin)
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0);   // Configure the channel to trigger on an edge
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0);     // Configure it to trigger on the falling edge
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH0);    // In this case, it triggers on the rising edge

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
