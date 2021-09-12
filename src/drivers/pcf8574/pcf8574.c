#include "pcf8574.h"

void pcf8574_write(uint8_t address, uint8_t data)
{
    tw_start();
    tw_write_sla(TW_SLA_W(address));
    tw_write(data);
    tw_stop();
}

uint8_t pcf8574_read(uint8_t address)
{
    tw_start();
    tw_write_sla(TW_SLA_R(address));
    uint8_t result = tw_read(TW_READ_NACK);
    tw_stop();
    return result;
}

void pcf8574_write_pin(uint8_t address, uint8_t pin, uint8_t state)
{
    uint8_t port = pcf8574_read(address);
    if (state) {
        port |= (1 << pin);
    } else {
        port &= ~(1 << pin);
    }
    pcf8574_write(address, port);
}

uint8_t pcf8574_read_pin(uint8_t address, uint8_t pin)
{
    uint8_t port = pcf8574_read(address);
    return (port >> pin) & 1;
}