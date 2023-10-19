/**
 * @file semaphore.h
 * @author Marcos Dominguez
 * 
 * @brief Semaphore class.
 * 
 * @version 0.1
 * @date 2023-09-28
 */

#ifndef __OS_SEMAPHORE_H
#define __OS_SEMAPHORE_H

#include "os_queue.h"

/* ========= [Definition of public macros] =========================================== */

#define SEMPH_COUNTS(counts)      (counts > MAX_SEMPH_COUNT ? MAX_SEMPH_COUNT : (counts == 0 ? 1 : counts))  ///< keep max count range within 1 and MAX_SEMPH_COUNT

/* ========= [Definition of public data types] ======================================= */

/**
 * @brief Semaphore class atributes.
 */
typedef struct {
    uint8_t mem_buffer[MAX_SEMPH_COUNT];    ///< Buffer that storage semaphore counting.
    queue_t semaphore;                      ///< Queue object.
    uint8_t data;                           ///< Empty data to copy in queue logic.
} semaphore_t;                              

/* ========= [Definition of public methods] ========================================== */

/**
 * @brief Initializes semaphore binary or not.
 *
 * @param[in,out]   semph_ptr   Pointer to semaphore handler.
 * @param[in]       maxCount    Maximum count value that can be reached.
 * @param[in]       count       The count value assigned to the semaphore when it is created.
 */
#define OS_SEMAPHORE_Create(semph_ptr, maxCount, count) OS_QUEUE_Create(&((semaphore_t *)semph_ptr)->semaphore, (queue_mem_t)((semaphore_t *)semph_ptr)->mem_buffer, sizeof(uint8_t), SEMPH_COUNTS(maxCount), SEMPH_COUNTS(maxCount) - count)

/**
 * @brief Give semaphore.
 *
 * @param[in,out]   semph_ptr   Pointer to semaphore handler.
 * @return Returns true if the semaphore has been given in this instance.
 * 
 */
#define OS_SEMAPHORE_Give(semph_ptr)                    OS_QUEUE_Send(&((semaphore_t *)semph_ptr)->semaphore, (void *)&((semaphore_t *)semph_ptr)->data, (tick_type_t)0)


/**
 * @brief Take semaphore.
 *
 * @param[in,out]   semph_ptr   Pointer to semaphore handler.
 * @param[in,out]   wait_time   Tick to wait until semaphore fail.
 *
 * @return Returns true if the semaphore could be taken.
 */
#define OS_SEMAPHORE_Take(semph_ptr, wait_time)         OS_QUEUE_Receive(&((semaphore_t *)semph_ptr)->semaphore, (void *)&((semaphore_t *)semph_ptr)->data, (tick_type_t)wait_time)

#endif /* __OS_SEMAPHORE_H */
