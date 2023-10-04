/**
 * @file os_queue.h
 * @author Marcos Dominguez
 * @brief
 * @version 0.1
 * @date 2023-10-03
 *
 *
 */

#ifndef __OS_QUEUE_H
#define __OS_QUEUE_H

#include "OS/os_kernel.h"


/* =========[Definition of public macros] =========================================== */

#define BYTES_OF_QUEUE(data_size, element_qty) (data_size * element_qty)

/* =========[Definition of public data types] ======================================= */

typedef uint8_t *queue_mem_t;

typedef struct {
    os_task_t *task_list[MAX_NUMBER_TASK];
    os_task_t **pop_task;
    os_task_t **push_task;
    queue_mem_t fifo_ptr;
    queue_mem_t pop_element;
    queue_mem_t push_element;
    uint8_t data_size;
    uint8_t n_elements;
    uint8_t used_elements;
} queue_t;

/* =========[Definition of public methods]========================================== */

/**
 * @brief Initialize the queue.
 *
 * @param[in, out]  queue_obj                 Queue object.
 * @param[in]       queue_store_ptr           Address of ram where queue elements will be storaged.
 * @param[in]       data_size                 Data size of the queue.
 * @param[in]       queue_elements            Data size of the queue.
 * @param[in]       starting_used_elements    elemens of the queue used.
 *
 * @return Returns true if was success in otherwise false.
 */
bool OS_QUEUE_Create(queue_t *queue_obj, queue_mem_t queue_store_ptr, uint8_t data_size, uint8_t queue_elements, uint8_t starting_used_elements);

/**
 * @brief Send data to the queue.
 *
 * @param[in, out]  queue   Queue object.
 * @param[in, out]  data    Data sent to the queue.
 * @param[in]       timeout Number of ticks to wait before blocking the task..
 *
 * @return Returns true if it could be put in the queue
 * in otherwise false.
 */
bool OS_QUEUE_Send(queue_t *queue_obj, const void *data, const tick_type_t timeout);

/**
 * @brief Receive data to the queue.
 *
 * @param[in, out]  queue   Queue object.
 * @param[in, out]  buffer  Buffer to  save the data read from the queue.
 * @param[in]       timeout Number of ticks to wait before blocking the task..
 *
 * @return Returns true if it was possible to take it out in the queue
 * in otherwise false.
 */
bool OS_QUEUE_Receive(queue_t *queue_obj, void *data, const tick_type_t timeout);

#endif /* __OS_QUEUE_H */
