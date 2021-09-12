#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <stdbool.h>

#ifndef BAUD
    #define BAUD 9600
#endif

#define UART_BUFFER_SIZE 16

/** Stream that outputs to the UART and reads from it. */
extern FILE uart_io;

/** Stream that outputs to the UART and reads from it immediately. */
extern FILE uart_immediate_io;

void uart_init();

bool uart_transmit(uint8_t data);

bool uart_receive(uint8_t* data);
uint8_t uart_receiveAvailable(void);
bool uart_receivePeek(uint8_t *data);

char* uartint_ngets(char *s, size_t n);

uint8_t uart_receive_immediately(void);
void uart_transmit_immediately(uint8_t data);

#endif