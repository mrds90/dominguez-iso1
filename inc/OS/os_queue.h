/**
 * @file os_queue.h
 * @author Marcos Dominguez
 * 
 * @brief Queue class.
 * 
 * @version 0.1
 * @date 2023-10-03
 */

#ifndef __OS_QUEUE_H
#define __OS_QUEUE_H

#include "os_kernel.h"

/* =========[Definition of public macros] =========================================== */

#define BYTES_OF_QUEUE(data_size, element_qty) (data_size * element_qty) ///< Bytes used by a queue to storage the data.

/* =========[Definition of public data types] ======================================= */

typedef uint8_t *queue_mem_t;                   ///< Pointer to queue data region in ram.

/**
 * @brief Queue class atributes.
 */
typedef struct {
    os_task_t *task_list[MAX_NUMBER_TASK];      ///< Task blocked by the queue.
    os_task_t **pop_task;                       ///< Pointer of task to unblock.
    os_task_t **push_task;                      ///< Pointer where blocked task pointer will be storaged.
    queue_mem_t fifo_ptr;                       ///< Pointer to ram memory addr where queue data is storaged.
    queue_mem_t pop_element;                    ///< Address where next element will be popped-out.
    queue_mem_t push_element;                   ///< Address where next element will be pushed-out.
    uint8_t data_size;                          ///< Size of elements queued.
    uint8_t n_elements;                         ///< Elements quantity that can be queued.
    uint8_t used_elements;                      ///< Elements quantity that has been queued.
} queue_t;                                      

/* =========[Definition of public methods]========================================== */

/**
 * @brief Initialize the queue.
 *
 * @code
 * 
 * // ============ [Example of usage] =================
 * 
 * #define N_QUEUE_ELEMETS 50
 * 
 * static queue_t queue_obj;
 * 
 * static uint16_t ram_mem[N_QUEUE_ELEMETS];
 * 
 * OS_QUEUE_Create(&queue_obj, (queue_mem_t) &ram_mem, sizeof(uint16_t), N_QUEUE_ELEMETS, 0);
 * 
 * @endcode
 * 
 * @param[in, out]  queue_obj                 Queue object.
 * @param[in]       queue_store_ptr           Address of ram where queue elements will be storaged.
 * @param[in]       data_size                 Data size of the queue element.
 * @param[in]       queue_elements            Queue element quantity.
 * @param[in]       starting_used_elements    Elements of the queue used at creation (filled with 0).
 *  
 * 
 * @return Returns true if was success in otherwise false.
 */
bool OS_QUEUE_Create(queue_t *queue_obj, queue_mem_t queue_store_ptr, uint8_t data_size, uint8_t queue_elements, uint8_t starting_used_elements);

/**
 * @brief Send data to the queue.
 *
 * @param[in, out]  queue   Queue object.
 * @param[in, out]  data    Data sent to the queue.
 * @param[in]       timeout Number of ticks to wait before unblocking the task.
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
 * @param[in]       timeout Number of ticks to wait before unblocking the task.
 *
 * @return Returns true if it was possible to take it out in the queue
 * in otherwise false.
 */
bool OS_QUEUE_Receive(queue_t *queue_obj, void *data, const tick_type_t timeout);

#endif /* __OS_QUEUE_H */
