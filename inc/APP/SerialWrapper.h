/**
 * @file SerialWrapper.h
 * @author Marcos Dominguez
 * 
 * @brief EDU-CIAA Hardware abstraction layer for Serial Port.
 * 
 * @version 0.1
 * @date 2023-10-19
 */

#ifndef INC_SERIALWRAPPER_H_
#define INC_SERIALWRAPPER_H_

#include <stdint.h>

/* ========= [Definition of public macros] =========================================== */

/* ========= [Definition of public data types] ======================================= */

/* ========= [Definition of public methods] ========================================== */

/**
 * @brief Print by USB_UART an string.
 * 
 * @param buffer String with message to be send.
 */
void serialPrint(const char* buffer);

#endif /* INC_SERIALWRAPPER_H_ */
