#ifndef UART_PROTO_H
#define UART_PROTO_H

#include "generic_types.h"
#include "uart_types.h"

/* Get the address of the memory mapped register for UART */
volatile  UART_Reg_def_t* UART_GetPort();

/* Initialise a UART peripheral */
void      USART_Init();

/* API to receive data via UART port */
uint16_t  uart_receive();

/* API to write data into the UART port */
void      uart_write(uint8_t ch);

#endif /* UART_PROTO_H */
