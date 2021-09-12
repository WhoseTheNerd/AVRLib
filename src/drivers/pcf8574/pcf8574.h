#ifndef PCF8574_H
#define PCF8574_H

#include <avr/io.h>
#include "../twi/twi_master.h"

void pcf8574_write(uint8_t address, uint8_t data);
uint8_t pcf8574_read(uint8_t address);

void pcf8574_write_pin(uint8_t address, uint8_t pin, uint8_t state);
uint8_t pcf8574_read_pin(uint8_t address, uint8_t pin);

#endif /* PCF8574_H */