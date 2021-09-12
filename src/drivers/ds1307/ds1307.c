#include "ds1307.h"

#include "../twi/twi_master.h"

#include <avr/pgmspace.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>

#define DS1307_RAM_OFFSET 8

uint8_t bcd2dec(uint8_t bcd);
uint8_t dec2bcd(uint8_t dec);

void ds1307_clock_write(uint8_t address, struct Time time)
{
    tw_start();
    tw_write_sla(TW_SLA_W(address));
    tw_write(0);
    tw_write(dec2bcd(time.second));
    tw_write(dec2bcd(time.minute));
    tw_write(dec2bcd(time.hour));
    tw_stop();
}

void ds1307_calendar_write(uint8_t address, struct Date date)
{
    tw_start();
    tw_write_sla(TW_SLA_W(address));
    tw_write(3);
    tw_write(dec2bcd(date.day));
    tw_write(dec2bcd(date.date));
    tw_write(dec2bcd(date.month));
    tw_write(dec2bcd(date.year));
    tw_stop();
}

void ds1307_clock_read(uint8_t address, struct Time* time)
{
    tw_start();
    tw_write_sla(TW_SLA_W(address));
    tw_write(0);
    tw_start();
    tw_write_sla(TW_SLA_R(address));
    time->second = bcd2dec(tw_read(TW_READ_ACK));
    time->minute = bcd2dec(tw_read(TW_READ_ACK));
    time->hour = bcd2dec(tw_read(TW_READ_NACK));
    tw_stop();

    if (time->second & (1 << 7))
    {
        time->halted = true;
        time->second &= ~(1 << 7);
    }
    else
        time->halted = false;
    
    if (time->hour & (1 << 6))
    {
        time->ampm_mode = true;
        time->ampm = time->hour & (1 << 5);
        time->hour &= ~(1 << 5);
    }
    else
        time->ampm_mode = false;   
}

void ds1307_calendar_read(uint8_t address, struct Date* date)
{
    tw_start();
    tw_write_sla(TW_SLA_W(address));
    tw_write(3);
    tw_start();
    tw_write_sla(TW_SLA_R(address));
    date->day = bcd2dec(tw_read(TW_READ_ACK));
    date->date = bcd2dec(tw_read(TW_READ_ACK));
    date->month = bcd2dec(tw_read(TW_READ_ACK));
    date->year = bcd2dec(tw_read(TW_READ_NACK));
    tw_stop();
}

void time_parse(const char* time_format, struct Time* time)
{
    char* strHours = strtok_P((char*)time_format, PSTR(":"));
    char* strMinutes = strtok_P(NULL, PSTR(":"));
    char* strSeconds = strtok_P(NULL, PSTR(":"));

    time->hour = atoi(strHours);
    time->minute = atoi(strMinutes);
    time->second = atoi(strSeconds);
}

uint8_t wd(int32_t year, int32_t month, int32_t day);

void date_parse(const char* date_format, struct Date* date)
{
    char* strMonth = strtok_P((char*)date_format, PSTR(" "));
    char* strDay = strtok_P(NULL, PSTR(" "));
    char* strYear = strtok_P(NULL, PSTR(" "));

    if (strcmp_P(strMonth, PSTR("Jan")) == 0)
    {
        date->month = 1;
    }
    else if (strcmp_P(strMonth, PSTR("Feb")) == 0)
    {
        date->month = 2;
    }
    else if (strcmp_P(strMonth, PSTR("Mar")) == 0)
    {
        date->month = 3;
    }
    else if (strcmp_P(strMonth, PSTR("Apr")) == 0)
    {
        date->month = 4;
    }
    else if (strcmp_P(strMonth, PSTR("May")) == 0)
    {
        date->month = 5;
    }
    else if (strcmp_P(strMonth, PSTR("Jun")) == 0)
    {
        date->month = 6;
    }
    else if (strcmp_P(strMonth, PSTR("Jul")) == 0)
    {
        date->month = 7;
    }
    else if (strcmp_P(strMonth, PSTR("Aug")) == 0)
    {
        date->month = 8;
    }
    else if (strcmp_P(strMonth, PSTR("Sep")) == 0 || strcmp_P(strMonth, PSTR("Sept")) == 0)
    {
        date->month = 9;
    }
    else if (strcmp_P(strMonth, PSTR("Oct")) == 0)
    {
        date->month = 10;
    }
    else if (strcmp_P(strMonth, PSTR("Nov")) == 0)
    {
        date->month = 11;
    }
    else if (strcmp_P(strMonth, PSTR("Dev")) == 0)
    {
        date->month = 12;
    }

    date->date = atoi(strDay);
    date->year = atoi(strYear) - 2000;
    date->day = wd(atoi(strYear), date->month, date->date);
}

uint8_t wd(int32_t year, int32_t month, int32_t day) 
{
  /* using C99 compound literals in a single line: notice the splicing */
  return ((uint8_t[])                                         \
          {2, 3, 4,                       \
           5, 6, 7, 1})[           \
      (                                                            \
          day                                                      \
        + ((153 * (month + 12 * ((14 - month) / 12) - 3) + 2) / 5) \
        + (365 * (year + 4800 - ((14 - month) / 12)))              \
        + ((year + 4800 - ((14 - month) / 12)) / 4)                \
        - ((year + 4800 - ((14 - month) / 12)) / 100)              \
        + ((year + 4800 - ((14 - month) / 12)) / 400)              \
        - 32045                                                    \
      ) % 7];
}

uint8_t bcd2dec(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd % 16);
}

uint8_t dec2bcd(uint8_t dec)
{
    return ((dec / 10) * 16) + (dec % 10);
}

bool is_rtc_behind(struct Time current, struct Time rtc)
{
    return current.second > rtc.second || current.minute > rtc.minute || current.hour > rtc.hour;
}

void ds1307_write_ram(uint8_t address, uint8_t ram_address, uint8_t data)
{
    if (ram_address > 56)
    {
        ram_address = 56;
        puts_P(PSTR("DS1307 NVRAM has 56 bytes available.\n"));
    }
    tw_start();
    tw_write_sla(TW_SLA_W(address));
    tw_write(ram_address + DS1307_RAM_OFFSET);
    tw_write(data);
    tw_stop();
}

uint8_t ds1307_read_ram(uint8_t address, uint8_t ram_address)
{
    if (ram_address > 56) 
    {
        ram_address = 56;
        puts_P(PSTR("DS1307 NVRAM has 56 bytes available.\n"));
    }
    tw_start();
    tw_write_sla(TW_SLA_W(address));
    tw_write(ram_address + DS1307_RAM_OFFSET);
    tw_start();
    tw_write_sla(TW_SLA_R(address));
    uint8_t data = tw_read(TW_READ_NACK);
    tw_stop();

    return data;
}