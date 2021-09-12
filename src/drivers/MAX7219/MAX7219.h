#pragma once

#include <avr/io.h>

#define MAX7219_CHIPS 1

/* MAX7219 Registers */
enum {
    MAX7219_REG_NOOP        = 0x0,
    MAX7219_REG_DECODEMODE  = 0x9,
    MAX7219_REG_INTENSITY   = 0xA,
    MAX7219_REG_SCANLIMIT   = 0xB,
    MAX7219_REG_SHUTDOWN    = 0xC,
    MAX7219_REG_DISPLAYTEST = 0xF,
};

void MAX7219_init(void);
void MAX7219_send(uint8_t segment, uint8_t data);