/**
 * @file GPIOWrapper.c
 * @author Marcos Dominguez
 * 
 * @brief EDU-CIAA Hardware abstraction layer for GPIO.
 * 
 * @version 0.1
 * @date 2023-10-19
 */

#include "GPIOWrapper.h"
#include "chip.h"

/* ==================== Define private variables ==================== */

/* ================== Private variables declaration ================= */

/* ================== Private functions declaration ================= */

/* ================= Public functions implementation ================ */

bool __HAL_GPIO_EXTI_GET_IT(uint32_t button) {
    uint32_t button_map;
    switch (button) {
        case TEC1:
            button_map = PININTCH0;
            break;

        case TEC2:
            button_map = PININTCH1;
            break;

        case TEC3:
            button_map = PININTCH2;
            break;

        case TEC4:
            button_map = PININTCH3;
            break;

        default:
            button_map = 0;
            break;
    }
    bool ret = false;
    if ((Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT) & button_map) || (Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & button_map)) {
        ret = true;
    }
    return ret;
}

void __HAL_GPIO_EXTI_CLEAR_IT(uint32_t button) {
    uint32_t button_map;
    switch (button) {
        case TEC1:
            button_map = PININTCH0;
            break;

        case TEC2:
            button_map = PININTCH1;
            break;

        case TEC3:
            button_map = PININTCH2;
            break;

        case TEC4:
            button_map = PININTCH3;
            break;

        default:
            button_map = 0;
            break;
    }
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, button_map);
}

bool gpioGetLevel(uint16_t pin, uint32_t port) {
    bool ret = false;
    if (gpioRead(pin)) {
        ret = true;
    }
    return ret;
}
