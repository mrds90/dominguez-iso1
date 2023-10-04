#ifndef INC_OSOS_SEMAPHORE_H
#define INC_OSOS_SEMAPHORE_H

#include "OS/os_semaphore.h"

typedef semaphore_t osSemaphoreObject;

/**
 * @brief Initializes semaphore binary or not.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 * @param[in]       maxCount    Maximum count value that can be reached (up to MAX_SEMPH_COUNT). (if 0 or 1 selected is a binary semaphore).
 * @param[in]       count       The count value assigned to the semaphore when it is created (must be 0 to start taken).
 */
#define osSemaphoreInit(semaphore, maxCount, count) OS_SEMAPHORE_Create((osSemaphoreObject*) semaphore, maxCount, count)

/**
 * @brief Take semaphore.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 * 
 *
 * @return Returns true if the semaphore could be taken.
 */
#define osSemaphoreTake(semaphore)                  OS_SEMAPHORE_Take((osSemaphoreObject*) semaphore, MAX_DELAY)

/**
 * @brief Give semaphore.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 */
#define osSemaphoreGive(semaphore)                  OS_SEMAPHORE_Give((osSemaphoreObject*) semaphore)


#endif // INC_OSOS_SEMAPHORE_H
