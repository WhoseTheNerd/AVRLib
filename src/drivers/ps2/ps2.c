#include "ps2.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <string.h>

#include "drivers/ring_buffer/ring.h"
#include "drivers/uart/uart.h"

#define PS2_DATA 2
#define PS2_CLOCK 3

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

static volatile ring_t ps2_receiveBuf;
static volatile uint8_t ps2_receiveArray[PS2_BUFFER_SIZE];

static void clock_low()
{
	PORTD &= ~(1 << PS2_CLOCK);
	DDRD |= (1 << PS2_CLOCK);
}

static void clock_high()
{
	DDRD &= ~(1 << PS2_CLOCK);
	PORTD |= (1 << PS2_CLOCK);
}

static void data_low()
{
	PORTD &= ~(1 << PS2_DATA);
	DDRD |= (1 << PS2_DATA);
}

static void data_high()
{
	DDRD &= ~(1 << PS2_DATA);
	PORTD |= (1 << PS2_DATA);
}

void ps2_init(void)
{
    ps2_receiveBuf = ring_init((uint8_t*)ps2_receiveArray, PS2_BUFFER_SIZE);

    // Set PD2 as Input pullup
	DDRD &= ~(1 << 2);
	PORTD |= (1 << 2);

	// Setup interrupt on PS2 clock (INT1, pin 3)
	EICRA |= (1 << ISC11); // Falling edge on INT1
	EIMSK |= (1 << INT1); // Enable INT1 interrupt
}

static void enable_int1() {
    EIMSK |= (1 << INT1); 

	// cleans the buffer
	uint8_t temp;
	while (!ring_pop((ring_t*)&ps2_receiveBuf, &temp));
}

static void disable_int1() {
    EIMSK &= ~(1 << INT1); 
}

void ps2_write(uint8_t data)
{
    disable_int1();

	uint8_t parity = 1;

	data_high();
	clock_high();
	_delay_us(300);
	clock_low();
	_delay_us(300);
	data_low();
	_delay_us(10);
	clock_high();

	while(bitRead(PIND, PS2_CLOCK));

	for (uint8_t i = 0; i < 8; ++i) {
		if (data & 0x01) {
			data_high();
		} else {
			data_low();
		}
		while(!bitRead(PIND, PS2_CLOCK));
		while(bitRead(PIND, PS2_CLOCK));
		parity ^= data & 0x01;
		data >>= 1;
	}

	if (parity) {
		data_high();
	} else {
		data_low();
	}

	while(!bitRead(PIND, PS2_CLOCK));
	while(bitRead(PIND, PS2_CLOCK));

	data_high();
	_delay_us(50);
	while(bitRead(PIND, PS2_CLOCK));
    enable_int1();
}

ps2_error_code_t ps2_read(uint8_t* data)
{
    if (ring_pop((ring_t*)&ps2_receiveBuf, data)) {
        return PS2_ERROR_EMPTY;
    }
    
    return PS2_ERROR_NONE;
}

static uint8_t calculate_odd_parity(uint8_t data)
{
	uint8_t count = 0;
	for (uint8_t i = 0; i < 8; ++i) {
		if (data & (1 << i)) {
			count++;
		}
	}

	if (count % 2 == 0) return 1;

	return 0;
}

ISR(INT1_vect)
{
	static uint16_t data = 0;
	static uint8_t bit_index = 0;

	uint8_t bit = PIND & (1 << 2) ? 1 : 0;
	// wait for start bit
	if (bit_index == 0) {
		if (bit == 1)  {
			return;
		}
	}
	data |= (bit << bit_index);
	bit_index++;
	if (bit_index == 11) {
		bit_index = 0;

		/*
			0000011011101110
			BBBBBSPDDDDDDDDS

			B = blank, unused
			S = stop bit, always 1
			D = data bit
			S = start bit always 0 
			P = odd parity
		*/
		uint8_t real_data = (data >> 1) & 0x00FF;

		//                                  BBBBBSPDDDDDDDDS
		uint8_t received_parity = (data & 0b0000001000000000) >> 9;

		uint8_t calculated_parity = calculate_odd_parity(real_data);

		if (received_parity == calculated_parity) {
			ring_pushOver((ring_t*)&ps2_receiveBuf, real_data);
		} else {
			fprintf_P(&uart_immediate_io, PSTR("Parity check failed."));
			ps2_write(PS2_RESEND); // Failed to receive the packet because parity mismatch
		}

		data = 0;

		// if buffer size is 16 then 4 bit counter will roll over
		#if PS2_BUFFER_SIZE < 16
		if (buffer_index.offset_index >= PS2_BUFFER_SIZE) {
			buffer_index.offset_index = 0;
		}
		#endif
	}
}

void ps2_clear(void)
{
	memset(ps2_receiveArray, 0x00, PS2_BUFFER_SIZE);
	ps2_receiveBuf = ring_init((uint8_t*)ps2_receiveArray, PS2_BUFFER_SIZE);
}