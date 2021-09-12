#include "MAX7219.h"

#include "../spi/spi.h"

static void enable_spi(void)
{
    DDRB |= (1 << 2);
}

static void disable_spi(void)
{
    DDRB &= ~(1 << 2);
}

static void send_byte(uint8_t reg, uint8_t data)
{
    spi_writeRead(reg);
    spi_writeRead(data);
}

static void send_to_all(uint8_t count, uint8_t reg, uint8_t data)
{
    for (uint8_t i = 0; i < count; ++i) {
        send_byte(reg, data);
    }
}

void MAX7219_init(void)
{
    enable_spi();
    send_to_all(MAX7219_CHIPS, MAX7219_REG_SCANLIMIT, 7);
    send_to_all(MAX7219_CHIPS, MAX7219_REG_DECODEMODE, 0);
    send_to_all(MAX7219_CHIPS, MAX7219_REG_DISPLAYTEST, 0);
    send_to_all(MAX7219_CHIPS, MAX7219_REG_INTENSITY, 15);
    send_to_all(MAX7219_CHIPS, MAX7219_REG_SHUTDOWN, 1); // active low

    // clear display
    send_to_all(MAX7219_CHIPS, MAX7219_REG_NOOP, 0x00);
}

void MAX7219_send(uint8_t segment, uint8_t data)
{
    send_byte(segment, data);
}