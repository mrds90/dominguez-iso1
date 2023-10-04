/**
 * @file os_config.h
 * @author Marcos Dominguez
 * @brief Configure parameters of the OS
 * @version 0.1
 * @date 2023-09-24
 *
 *
 */

#ifndef __OS_CONFIG_H
#define __OS_CONFIG_H

/* =========[Definition of public macros] =========================================== */

#define MAX_NUMBER_TASK         9U                                  ///< Defines maximum task we could create.
#define MAX_STACK_SIZE          256U                                ///< Defines maximum stack size for a task.
#define PRIORITY_LEVELS         4U                                  ///< Define max priority level of the OS (max possible: 4)
#define SYSTICK_PERIOD_MS       1U                                  ///< Systick period time in mili-second.
#define MAX_SEMPH_COUNT         32U                                 ///< Max count of the semaphore.


/* =========[Definition of public data types] ======================================= */

/* =========[Definition of public methods]========================================== */


#endif /* __OS_CONFIG_H */
