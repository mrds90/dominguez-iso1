/**
 * @file SerialWrapper.c
 * @author Marcos Dominguez
 * 
 * @brief EDU-CIAA Hardware abstraction layer for Serial Port.
 * 
 * @version 0.1
 * @date 2023-10-19
 */

#include <string.h>
#include "SerialWrapper.h"
#include "sapi.h"

/* ==================== Define private variables ==================== */

/* ================== Private variables declaration ================= */

/* ================== Private functions declaration ================= */

/* ================= Public functions implementation ================ */

void serialPrint(const char *buffer) {
    uartWriteByteArray(UART_USB, buffer, strlen(buffer));
}
