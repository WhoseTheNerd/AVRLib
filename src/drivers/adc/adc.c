#include "adc.h"

#include <avr/io.h>

void adc_init()
{
    // Prescalar 128, 125KHz @ 16MHz
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS0); // AVCC
	ADCSRA |= (1 << ADEN) | (1 << ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
}

uint8_t adc_read_8bit(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADMUX |= (1 << ADLAR);

    ADCSRA |= (1 << ADSC); // Trigger conversion
    loop_until_bit_is_clear(ADCSRA, ADSC); // Conversion complete
    
    return ADCH;
}

uint16_t adc_read_10bit(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADMUX &= ~(1 << ADLAR);

    ADCSRA |= (1 << ADSC); // Trigger conversion
    loop_until_bit_is_clear(ADCSRA, ADSC); // Conversion complete

    return ADCL | (ADCH << 8);
}