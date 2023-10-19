/**
 * @file timer_config.h
 * @author Marcos Dominguez
 * @brief  Configure timer for lpc 18xx_43xx
 * @version 0.1
 * @date 2023-10-07
 *
 * @copyright Copyright (c) 2023
 */
/*=====[Avoid multiple inclusion - begin] ====================================*/
#ifndef TIMER_CONFIG_H
#define TIMER_CONFIG_H

/*=====[Inclusions of public function dependencies] ==========================*/

#include <stdint.h>

/*=====[C++ - begin] =========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants] ===============================*/

/*=====[Public function-like macros] =========================================*/

/*=====[Definitions of public data types] ====================================*/

/*=====[Prototypes (declarations) of public functions] =======================*/

/**
 * @brief Initialize a timer with a given match register value.
 *
 * @param frequency Frequency of interrupting of the timer (FREQ Range: 6250Hz to 400000 Hz)
 * @param callback Function to be called at interrupt.
 */
void TIMER_CONFIG_Init(uint32_t frequency, void* callback);

/**
 * @brief Reset the count of the timer.
 */
void TIMER_CONFIG_Reset(void);
/**
 * @brief Get the elapsed time in microseconds since the timer was started.
 *
 * @return Elapsed time in microseconds.
 */
uint32_t TIMER_CONFIG_GetElapsedTimeUs(void);







/*=====[Prototypes (declarations) of public interrupt functions] =============*/

/*=====[C++ - end] ===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end] ======================================*/
#endif /* TIMER_CONFIG_H */
