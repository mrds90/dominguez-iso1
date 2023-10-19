#include <string.h>

#include "SerialWrapper.h"
#include "sapi.h"

void serialPrint(const char *buffer) {
    uartWriteByteArray(UART_USB, buffer, strlen(buffer));
}
