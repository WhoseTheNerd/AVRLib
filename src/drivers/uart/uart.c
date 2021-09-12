#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <util/atomic.h>
#include <stdbool.h>

#include "../ring_buffer/ring.h"

static int uart_putc(char c, FILE *stream)
{
    (void)stream;   //suppress unused warning
    if(uart_transmit(c))
        return _FDEV_EOF;
    return c;
}

static int uart_getc(FILE *stream)
{
    (void)stream;   //suppress unused warning
    
    uint8_t c = 0;
    if (uart_receive(&c))
        return _FDEV_EOF;

    return c;
}

FILE uart_io = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);

static int uart_immediate_putc(char c, FILE *stream)
{
    (void)stream;   //suppress unused warning
    uart_transmit_immediately(c);
    return 0;
}

static int uart_immediate_getc(FILE *stream)
{
    (void)stream;   //suppress unused warning
    return uart_receive_immediately();
}

FILE uart_immediate_io = FDEV_SETUP_STREAM(uart_immediate_putc, uart_immediate_getc, _FDEV_SETUP_RW);

static volatile ring_t uart_transmitBuf, uart_receiveBuf;
static volatile uint8_t uart_transmitArray[UART_BUFFER_SIZE], 
                        uart_receiveArray[UART_BUFFER_SIZE];

void uart_init()
{
    uart_transmitBuf = ring_init((uint8_t*)uart_transmitArray, UART_BUFFER_SIZE);
    uart_receiveBuf = ring_init((uint8_t*)uart_receiveArray, UART_BUFFER_SIZE);

    //setbaud.h values
    //https://www.nongnu.org/avr-libc/user-manual/group__util__setbaud.html
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    #if USE_2X
    USCR0A |= (1 << U2X0);
    #endif

    UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

    stdout = &uart_io;
    stdin = &uart_io;
}

char *uartint_ngets(char *s, size_t n)
{
    uint8_t c = 0;
    //next index to write to
    static size_t i = 0;
    
    while(!uart_receive(&c))
    {
        s[i++] = c;
        
        if(c == '\n' || i >= n-1)
        {
            s[i] = '\0';
            i = 0;
            
            return s;
        }
    }
    
    return NULL;
}

/**
 * Returns true if a byte has been received and is ready to be read.
 * Will be reset when the data register UDR0 is read.
 * 
 * @return if a byte has been received and is ready to be read
 */
static bool uart_isReceiveComplete(void)
{
    return UCSR0A & (1 << RXC0);
}

/**
 * Returns true when a transmission is completed.
 * 
 * @returns if a transmission is completed
 */
static bool uart_isTransmitComplete(void)
{
    return UCSR0A & (1 << TXC0);
}

/**
 * Returns true when the data register UDR0 is empty
 * and new data can be written into it.
 * 
 * @returns if the data register UDR0 is empty
 */
static bool uart_isDataEmpty(void)
{
    return UCSR0A & (1 << UDRE0);
}

void uart_transmit_immediately(uint8_t data)
{
    while(!uart_isDataEmpty());
    
    UDR0 = data;
    
    //wait for completed transmission so the user can be sure
    //that the byte is completely clocked out
    //and hopefully has been received by someone
    while(!uart_isTransmitComplete());
}

uint8_t uart_receive_immediately(void)
{
    while(!uart_isReceiveComplete());
    
    return UDR0;
}

bool uart_transmit(uint8_t data)
{
    //wait for available location
    while(ring_isFull(uart_transmitBuf));
    
    bool ret = false;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ret = ring_push((ring_t*)&uart_transmitBuf, data);
    }
    
    //only start transmitter when bytes has been written to the fifo
    if(!ret)
        UCSR0B |= (1 << UDRIE0);
    
    return ret;
}

uint8_t uart_receiveAvailable(void)
{
    size_t ret = 0;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ret = ring_popAvailable(uart_receiveBuf);
    }
    
    return ret;
}

bool uart_receivePeek(uint8_t *data)
{
    bool ret = false;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ret = ring_peek((ring_t*)&uart_receiveBuf, data);
    }
    
    return ret;
}

bool uart_receive(uint8_t* data)
{
    bool ret = false;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ret = ring_pop((ring_t*)&uart_receiveBuf, data);
    }
    
    return ret;
}

ISR(USART_UDRE_vect)
{
    uint8_t c = 0;
    
    if(!ring_pop((ring_t*)&uart_transmitBuf, &c))
        UDR0 = c;
    //stop transmitter when there is not data to be transmitted
    else
        UCSR0B &= ~(1 << UDRIE0);
}

ISR(USART_RX_vect)
{
    #ifdef UART_OVERWRITE
        ring_pushOver((ring_t*)&uart_receiveBuf, UDR0);
    #else
        ring_push((ring_t*)&uart_receiveBuf, UDR0);
    #endif
}