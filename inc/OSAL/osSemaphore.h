#ifndef INC_OSOS_SEMAPHORE_H
#define INC_OSOS_SEMAPHORE_H

#include "OS/os_semaphore.h"

typedef semaphore_t osSemaphoreObject;

/**
 * @brief Initializes semaphore binary or not.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 * @param[in]       maxCount    Maximum count value that can be reached (NOT IMPLEMENTED).
 * @param[in]       count       The count value assigned to the semaphore when it is created (NOT IMPLEMENTED).
 */
#define osSemaphoreInit(semaphore, maxCount, count) OS_SEMAPHORE_CreateBinary((osSemaphoreObject*) semaphore)

/**
 * @brief Take semaphore.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 * @param[in,out]   wait_time   Tick to wait until semaphore fail.
 *
 * @return Returns true if the semaphore could be taken.
 */
#define osSemaphoreTake(semaphore, wait_time)       OS_SEMAPHORE_Take((osSemaphoreObject*) semaphore, wait_time)

/**
 * @brief Give semaphore.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 */
#define osSemaphoreGive(semaphore)                  OS_SEMAPHORE_Give((osSemaphoreObject*) semaphore)


#endif // INC_OSOS_SEMAPHORE_H