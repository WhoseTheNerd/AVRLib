#pragma once

#include <inttypes.h>
#include <stdbool.h>

#define PS2_BUFFER_SIZE 16

#define PS2_RESET 0xFF
#define PS2_RESEND 0xFE
#define PS2_SETDEFAULT 0xF6
#define PS2_DISABLE 0xF5
#define PS2_ENABLE 0xF4
#define PS2_SETRATEDELAY 0xF3
#define PS2_READID 0xF2
#define PS2_SETSCANCODESET 0xF0
#define PS2_ECHO 0xEE
#define PS2_SETLEDS 0xED

#define PS2_EXTENDED 0xE0
#define PS2_RELEASED 0xF0

typedef enum {
    PS2_ERROR_NONE,
    PS2_ERROR_EMPTY
} ps2_error_code_t;

void ps2_init(void);
void ps2_write(uint8_t data);
ps2_error_code_t ps2_read(uint8_t* data);
void ps2_clear(void);