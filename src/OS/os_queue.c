/**
 * @file os_queue.c
 * @author Marcos Dominguez
 * @brief
 * @version 0.1
 * @date 2023-10-03
 *
 *
 */

#include <string.h>

#include "OS/os_queue.h"
#include "OS/delay.h"
#include "OS/os_methods.h"
/* ==================== Define private variables ==================== */

/* ================== Private variables declaration ================= */

/* ================== Private functions declaration ================= */

/* ================= Public functions implementation ================ */

bool OS_QUEUE_Create(queue_t *queue_obj, queue_mem_t queue_store_ptr, uint8_t data_size, uint8_t queue_elements, uint8_t starting_used_elements) {
    bool ret = false;
    if (queue_obj != NULL) {
        if ((queue_store_ptr != NULL)) {
            if (starting_used_elements > queue_elements) {
                starting_used_elements = queue_elements;
            }
            queue_obj->pop_task = queue_obj->task_list;
            queue_obj->push_task = queue_obj->task_list;
            queue_obj->fifo_ptr = queue_store_ptr;
            queue_obj->push_element = queue_obj->fifo_ptr;
            queue_obj->push_element += starting_used_elements;
            queue_obj->pop_element = queue_obj->fifo_ptr;
            queue_obj->data_size = data_size;
            queue_obj->n_elements = queue_elements;
            queue_obj->used_elements = starting_used_elements;
            ret = true;
        }
    }
    return ret;
}

bool OS_QUEUE_Send(queue_t *queue_obj, const void *data, const tick_type_t timeout) {
    bool ret = false;
    if (queue_obj->used_elements >= queue_obj->n_elements) {
        os_task_t *current_task = OS_METHODS_GetCurrentTask();
        current_task->status = OS_TASK_BLOCK;
        *queue_obj->push_task = current_task;

        queue_obj->push_task++;
        if (queue_obj->push_task == &queue_obj->task_list[MAX_NUMBER_TASK]) {
            queue_obj->push_task = queue_obj->task_list;
        }
        OS_KERNEL_Delay(timeout);
    }
    if (queue_obj->used_elements < queue_obj->n_elements) {
        memcpy(queue_obj->push_element, data, queue_obj->data_size);
        queue_obj->push_element += queue_obj->data_size;
        if (queue_obj->push_element - (queue_obj->fifo_ptr - 1) >= BYTES_OF_QUEUE(queue_obj->data_size, queue_obj->n_elements)) {
            queue_obj->push_element = queue_obj->fifo_ptr;
        }
        queue_obj->used_elements++;

        if (queue_obj->pop_task != queue_obj->push_task) {
            os_task_t *task_handler = *queue_obj->pop_task;
            *queue_obj->pop_task++;
            if (queue_obj->pop_task == &queue_obj->task_list[MAX_NUMBER_TASK]) {
                queue_obj->pop_task = queue_obj->task_list;
            }
            OS_METHODS_SetTaskAsReady(task_handler);
            if (task_handler->priority > OS_METHODS_GetCurrentTask()->priority) {
                OS_KERNEL_PortYield();
            }
        }
        ret = true;
    }
    return ret;
}

bool OS_QUEUE_Receive(queue_t *queue_obj, void *data, const tick_type_t timeout) {
    bool ret = false;
    if (queue_obj->used_elements == 0) {
        os_task_t *current_task = OS_METHODS_GetCurrentTask();
        current_task->status = OS_TASK_BLOCK;
        *queue_obj->push_task = current_task;

        queue_obj->push_task++;
        if (queue_obj->push_task == &queue_obj->task_list[MAX_NUMBER_TASK]) {
            queue_obj->push_task = queue_obj->task_list;
        }
        OS_KERNEL_Delay(timeout);
    }
    if ((queue_obj->used_elements > 0) && (data!= NULL)) {
        memcpy(data, queue_obj->pop_element, queue_obj->data_size);
        queue_obj->pop_element += queue_obj->data_size;
        if (queue_obj->pop_element - (queue_obj->fifo_ptr - 1) >= BYTES_OF_QUEUE(queue_obj->data_size, queue_obj->n_elements)) {
            queue_obj->pop_element = queue_obj->fifo_ptr;
        }
        queue_obj->used_elements--;

        if (queue_obj->pop_task != queue_obj->push_task) {
            os_task_t *task_handler = *queue_obj->pop_task;
            *queue_obj->pop_task++;
            if (queue_obj->pop_task == &queue_obj->task_list[MAX_NUMBER_TASK]) {
                queue_obj->pop_task = queue_obj->task_list;
            }
            OS_METHODS_SetTaskAsReady(task_handler);
            if (task_handler->priority > OS_METHODS_GetCurrentTask()->priority) {
                OS_KERNEL_PortYield();
            }
        }
        ret = true;
    }
    return ret;
}

/* ================ Private functions implementation ================ */

/* ========== Processor Interruption and Exception Handlers ========= */
