/**
 * @file osSemaphore.h
 * @author Marcos Dominguez
 * 
 * @brief Semaphore API.
 * 
 * @version 0.1
 * @date 2023-10-08
 */

#ifndef INC_OSOS_SEMAPHORE_H
#define INC_OSOS_SEMAPHORE_H

#include "os_semaphore.h"

/* ========= [Definition of public macros] =========================================== */

/* ========= [Definition of public data types] ======================================= */

typedef semaphore_t osSemaphoreObject; ///< Attributes of semaphore class.

/* ========= [Definition of public methods] ========================================== */

/**
 * @brief Initializes semaphore binary or not.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 * @param[in]       maxCount    Maximum count value that can be reached (up to MAX_SEMPH_COUNT). If 0 or 1 selected is a binary semaphore.
 * @param[in]       count       The count value assigned to the semaphore when it is created (must be 0 to start taken).
 */
#define osSemaphoreInit(semaphore, maxCount, count) OS_SEMAPHORE_Create((osSemaphoreObject *) semaphore, maxCount, count)

/**
 * @brief Take semaphore.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 *
 *
 * @return Returns true if the semaphore could be taken.
 */
#define osSemaphoreTake(semaphore)                  OS_SEMAPHORE_Take((osSemaphoreObject *) semaphore, OS_MAX_DELAY)

/**
 * @brief Give semaphore.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 */
#define osSemaphoreGive(semaphore)                  OS_SEMAPHORE_Give((osSemaphoreObject *) semaphore)


#endif // INC_OSOS_SEMAPHORE_H
