#ifndef DS1307_H
#define DS1307_H

#include <avr/io.h>
#include <stdbool.h>

struct Time
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    bool ampm;
    bool ampm_mode;
    bool halted;
};

struct Date
{
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
};

void ds1307_clock_write(uint8_t address, struct Time time);
void ds1307_calendar_write(uint8_t address, struct Date date);

void ds1307_clock_read(uint8_t address, struct Time* time);
void ds1307_calendar_read(uint8_t address, struct Date* date);

void ds1307_write_ram(uint8_t address, uint8_t ram_address, uint8_t data);
uint8_t ds1307_read_ram(uint8_t address, uint8_t ram_address);

void time_parse(const char* time_format, struct Time* time);
void date_parse(const char* date_format, struct Date* date);

bool is_rtc_behind(struct Time current, struct Time rtc);

#endif