#ifndef INC_OSQUEUE_H
#define INC_OSQUEUE_H

#include "OS/os_queue.h"

#define MAX_SIZE_QUEUE  128     // Maximum buffer size of the queue

/**
 * @brief Data structure queue.
 */
typedef struct
{
    uint8_t queue_buffer[MAX_SIZE_QUEUE];
    queue_t queue;
} osQueueObject;


/**
 * @brief Initialize the queue.
 *
 * @param[in, out]  queue_ptr        Queue object.
 * @param[in]       dataSize         Data size of the queue.
 *
 * @return Returns true if was success in otherwise false.
 */
#define osQueueInit(queue_ptr, dataSize)            OS_QUEUE_Create(&((osQueueObject *)queue_ptr)->queue, (queue_mem_t)((osQueueObject *)queue_ptr)->queue_buffer, dataSize, MAX_SIZE_QUEUE / dataSize, 0)

/**
 * @brief Send data to the queue.
 *
 * @param[in, out]  queue_ptr   Queue object.
 * @param[in, out]  data        Data sent to the queue.
 * @param[in]       timeout     Number of ticks to wait before blocking the task..
 *
 * @return Returns true if it could be put in the queue
 * in otherwise false.
 */
#define osQueueSend(queue_ptr, data, timeout)       OS_QUEUE_Send(&((osQueueObject *)queue_ptr)->queue, (void *)data, (tick_type_t)timeout)

/**
 * @brief Receive data to the queue.
 *
 * @param[in, out]  queue_ptr   Queue object.
 * @param[in, out]  buffer      Buffer to  save the data read from the queue.
 * @param[in]       timeout     Number of ticks to wait before blocking the task..
 *
 * @return Returns true if it was possible to take it out in the queue
 * in otherwise false.
 */
#define osQueueReceive(queue_ptr, buffer, timeout)  OS_QUEUE_Receive(&((osQueueObject *)queue_ptr)->queue, (void *)buffer, (tick_type_t) timeout)

#endif // INC_OSQUEUE_H
