/**
 * @file GPIOWrapper.h
 * @author Marcos Dominguez
 * 
 * @brief EDU-CIAA Hardware abstraction layer for GPIO.
 * 
 * @version 0.1
 * @date 2023-10-19
 */

#ifndef INC_GPIOWRAPPER_H_
#define INC_GPIOWRAPPER_H_

#include <stdint.h>
#include <stdbool.h>

/* ========= [Definition of public macros] =========================================== */

#ifdef ARCH_NXP_LPC_43XX
#include "sapi.h"
#endif

#ifndef B1_Pin
#define B1_Pin                TEC1
#endif /* B1_Pin */

#ifndef B1_GPIO_Port
#define B1_GPIO_Port          0
#endif /* B1_GPIO_Port */

#ifndef B1_EXTI_IRQn
#define B1_EXTI_IRQn          PIN_INT0_IRQn
#endif /* B1_EXTI_IRQn */

#ifndef B2_Pin
#define B2_Pin                TEC2
#endif /* B2_Pin */

#ifndef B2_GPIO_Port
#define B2_GPIO_Port          0
#endif /* B2_GPIO_Port */

#ifndef B2_EXTI_IRQn
#define B2_EXTI_IRQn          PIN_INT1_IRQn
#endif /* B2_EXTI_IRQn */

#ifndef LD3_Pin
#define LD3_Pin               LED2
#endif /* LD3_Pin */

#ifndef LD3_GPIO_Port
#define LD3_GPIO_Port         0
#endif /* LD3_GPIO_Port */

#ifndef LD1_Pin
#define LD1_Pin               LED3
#endif /* LD1_Pin */

#ifndef LD1_GPIO_Port
#define LD1_GPIO_Port         0
#endif /* LD1_GPIO_Port */

#ifndef LD2_Pin
#define LD2_Pin               LEDB
#endif /* LD2_Pin */

#ifndef LD2_GPIO_Port
#define LD2_GPIO_Port         0
#endif /* LD2_GPIO_Port */

#ifndef LD4_Pin
#define LD4_Pin               LED1
#endif /* LD4_Pin */

#ifndef LD4_GPIO_Port
#define LD4_GPIO_Port         0
#endif /* LD4_GPIO_Port */

#ifndef Heartbeat_Pin
#define Heartbeat_Pin         LEDG
#endif /* Heartbeat_Pin */

#ifndef Heartbeat_GPIO_Port
#define Heartbeat_GPIO_Port   0
#endif /* Heartbeat_GPIO_Port */

#ifndef gpioSetLevel
#define gpioSetLevel(pin, port, value) gpioWrite((gpioMap_t)pin, (bool)value)
#endif /* gpioSetLevel */

/* ========= [Definition of public data types] ======================================= */

/* ========= [Definition of public methods] ========================================== */

/**
 * @brief Get the logical state of a specific pin.
 *
 * @param pin
 * @param port
 * @return true:  Button raised.
 * @return false: Button pressed.
 */
bool gpioGetLevel(uint16_t pin, uint32_t port);


#ifdef ARCH_NXP_LPC_43XX
/**
 * @brief Get if the Interrupt flag of an specific button is raised.
 *
 * @param button Button to be checked.
 * @return true: Flag is raised.
 * @return false: Flag is not raised.
 */
bool __HAL_GPIO_EXTI_GET_IT(uint32_t button);

/**
 * @brief Clear interrupt flag of linked to an specific button.
 *
 * @param button Button which interrupt shall be cleared.
 */
void __HAL_GPIO_EXTI_CLEAR_IT(uint32_t button);

#endif /* ARCH_NXP_LPC_43XX  */
#endif /* INC_GPIOWRAPPER_H_ */
