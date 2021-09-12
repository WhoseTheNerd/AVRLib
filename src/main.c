/* Hardware drivers */
#include "drivers/uart/uart.h"
#include "drivers/twi/twi_master.h"
#include "drivers/spi/spi.h"
#include "drivers/adc/adc.h"

/* Chip Drivers */
#include "drivers/TB6612/TB6612.h"

/* LIBC functions*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* AVR functions*/
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <util/delay.h>

#include "printf_helper.h"

#define DS1307_SLA 0x68
#define PCF8574_SLA 0x20

static void vehicle_forward(uint8_t speed, uint16_t duration)
{
	tb6612_stop();
	for (uint8_t i = 0; i < speed; i++) {
		tb6612_set1((int16_t)i);
		tb6612_set2((int16_t)i);
		_delay_us(10);
	}
	tb6612_set1((int16_t)speed);
	tb6612_set2((int16_t)speed);
	_delay_ms(duration - speed - (speed * 0.01f));
	while (speed --> 0) {
		tb6612_set1((int16_t)speed);
		tb6612_set2((int16_t)speed);
		_delay_ms(1);
	}
}

static void vehicle_backward(uint8_t speed, uint16_t duration)
{
	tb6612_stop();
	for (uint8_t i = 0; i < speed; i++) {
		tb6612_set1((int16_t)-i);
		tb6612_set2((int16_t)-i);
		_delay_us(10);
	}
	tb6612_set1((int16_t)-speed);
	tb6612_set2((int16_t)-speed);
	_delay_ms(duration - speed - (speed * 0.01f));
	while (speed --> 0) {
		tb6612_set1((int16_t)-speed);
		tb6612_set2((int16_t)-speed);
		_delay_ms(1);
	}
}

static void program_run(void)
{
	vehicle_forward(255, 500);
	vehicle_backward(255, 500);
	tb6612_stop();
}

int main()
{
	sei();
	uart_init();
	puts_P(PSTR("Hello, World!"));
	
	adc_init(); // required for A6/A7 input reading digital or analog

	tw_init(TW_FREQ_400K, true);

	uint8_t addresses[128];
	memset(addresses, 0, 128);
	tw_get_addresses(addresses);
	puts_P(PSTR("I2C Bus:"));
	for (uint8_t i = 0; i < 128; i += 16) {
		printf_P(PSTR("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x\n"), 
			addresses[i +  0], addresses[i +  1], addresses[i +  2], addresses[i +  3], 
			addresses[i +  4], addresses[i +  5], addresses[i +  6], addresses[i +  7],
			addresses[i +  8], addresses[i +  9], addresses[i + 10], addresses[i + 11],
			addresses[i + 12], addresses[i + 13], addresses[i + 14], addresses[i + 15]);
	}

	tb6612_init();


	while(1) {
		if (adc_read_8bit(7) == 0) {
			_delay_ms(2000);
			program_run();
		}
	}
}
