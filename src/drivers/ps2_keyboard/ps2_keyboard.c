#include "ps2_keyboard.h"

#include "drivers/ps2/ps2.h"
#include "drivers/uart/uart.h"

#include <avr/pgmspace.h>
#include <stdio.h>

static const __flash uint8_t normal[256] = {
    KB_NONE,KB_F9,KB_NONE,KB_F5,KB_F3,KB_F1,KB_F2,KB_F12,
    KB_NONE,KB_F10,KB_F8,KB_F6,KB_F4,KB_TAB,KB_GRAVE_ACCENT,
    KB_NONE,KB_NONE,KB_LEFT_ALT,KB_LEFT_SHIFT,KB_NONE,
    KB_LEFT_CTRL,KB_Q,KB_1,KB_NONE,KB_NONE,KB_NONE,KB_Z,
    KB_S,KB_A,KB_W,KB_2,KB_NONE,KB_NONE,KB_C,KB_X,KB_D,KB_E,
    KB_4,KB_3,KB_NONE,KB_NONE,KB_SPACEBAR,KB_V,KB_F,KB_T,KB_R,
    KB_5,KB_NONE,KB_NONE,KB_N,KB_B,KB_H,KB_G,KB_Y,KB_6,KB_NONE,KB_NONE,
    KB_NONE,KB_M,KB_J,KB_U,KB_7,KB_8,KB_NONE,KB_NONE,KB_COMMA,KB_K,KB_I,
    KB_O,KB_0,KB_9,KB_NONE,KB_NONE,KB_PERIOD,KB_FORWARD_SLASH,KB_L,KB_SEMICOLON,
    KB_P,KB_MINUS,KB_NONE,KB_NONE,KB_NONE,KB_APOSTROPHE,KB_NONE,KB_LEFT_BRACKET,
    KB_EQUALS,KB_NONE,KB_NONE,KB_CAPSLOCK,KB_RIGHT_SHIFT,KB_ENTER,KB_RIGHT_BRACKET,
    KB_NONE,KB_BLACKLASH,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_EQUALS,KB_NONE,KB_NONE,KB_NUMPAD_1,KB_NONE,KB_NUMPAD_4,KB_NUMPAD_7,
    KB_NONE,KB_NONE,KB_NONE,KB_NUMPAD_0,KB_NUMPAD_PERIOD,KB_NUMPAD_2,KB_NUMPAD_5,
    KB_NUMPAD_6,KB_NUMPAD_8,KB_ESC,KB_NUMLOCK,KB_F11,KB_NUMPAD_PLUS,KB_NUMPAD_3,
    KB_NUMPAD_MINUS,KB_NUMPAD_MULTIPLY,KB_NUMPAD_9,KB_SCROLL_LOCK,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_F7,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,
    KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE,KB_NONE
};

static uint8_t read_normal_table(uint8_t value)
{
    return pgm_read_byte(normal + value);
}

const __flash char* const __flash conversion[256] = {
 (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_ESC" },  (const __flash char[]) {"KB_F1" },  (const __flash char[]) {"KB_F2" },  (const __flash char[]) {"KB_F3" },
 (const __flash char[]) {"KB_F4" },  (const __flash char[]) {"KB_F5" },  (const __flash char[]) {"KB_F6" },  (const __flash char[]) {"KB_F7" },
 (const __flash char[]) {"KB_F8" },  (const __flash char[]) {"KB_F9" },  (const __flash char[]) {"KB_F10" },  (const __flash char[]) {"KB_F11" },
 (const __flash char[]) {"KB_F12" },  (const __flash char[]) {"KB_PRTSCR" },  (const __flash char[]) {"KB_SCROLL_LOCK" },  (const __flash char[]) {"KB_PAUSE_BREAK" },
 (const __flash char[]) {"KB_GRAVE_ACCENT" },  (const __flash char[]) {"KB_1" },  (const __flash char[]) {"KB_2" },  (const __flash char[]) {"KB_3" },
 (const __flash char[]) {"KB_4" },  (const __flash char[]) {"KB_5" },  (const __flash char[]) {"KB_6" },  (const __flash char[]) {"KB_7" },
 (const __flash char[]) {"KB_8" },  (const __flash char[]) {"KB_9" },  (const __flash char[]) {"KB_0" },  (const __flash char[]) {"KB_MINUS" },
 (const __flash char[]) {"KB_EQUALS" },  (const __flash char[]) {"KB_BACKSPACE" },  (const __flash char[]) {"KB_TAB" },  (const __flash char[]) {"KB_Q" },
 (const __flash char[]) {"KB_W" },  (const __flash char[]) {"KB_E" },  (const __flash char[]) {"KB_R" },  (const __flash char[]) {"KB_T" },
 (const __flash char[]) {"KB_Y" },  (const __flash char[]) {"KB_U" },  (const __flash char[]) {"KB_I" },  (const __flash char[]) {"KB_O" },
 (const __flash char[]) {"KB_P" },  (const __flash char[]) {"KB_LEFT_BRACKET" },  (const __flash char[]) {"KB_RIGHT_BRACKET" },  (const __flash char[]) {"KB_BLACKLASH" },
 (const __flash char[]) {"KB_CAPSLOCK" },  (const __flash char[]) {"KB_A" },  (const __flash char[]) {"KB_S" },  (const __flash char[]) {"KB_D" },
 (const __flash char[]) {"KB_F" },  (const __flash char[]) {"KB_G" },  (const __flash char[]) {"KB_H" },  (const __flash char[]) {"KB_J" },
 (const __flash char[]) {"KB_K" },  (const __flash char[]) {"KB_L" },  (const __flash char[]) {"KB_SEMICOLON" },  (const __flash char[]) {"KB_APOSTROPHE" },
 (const __flash char[]) {"KB_ENTER" },  (const __flash char[]) {"KB_LEFT_SHIFT" },  (const __flash char[]) {"KB_Z" },  (const __flash char[]) {"KB_X" },
 (const __flash char[]) {"KB_C" },  (const __flash char[]) {"KB_V" },  (const __flash char[]) {"KB_B" },  (const __flash char[]) {"KB_N" },
 (const __flash char[]) {"KB_M" },  (const __flash char[]) {"KB_COMMA" },  (const __flash char[]) {"KB_PERIOD" },  (const __flash char[]) {"KB_FORWARD_SLASH" },
 (const __flash char[]) {"KB_RIGHT_SHIFT" },  (const __flash char[]) {"KB_LEFT_CTRL" },  (const __flash char[]) {"KB_LEFT_HOST" },  (const __flash char[]) {"KB_LEFT_ALT" },
 (const __flash char[]) {"KB_SPACEBAR" },  (const __flash char[]) {"KB_RIGHT_ALT" },  (const __flash char[]) {"KB_RIGHT_HOST" },  (const __flash char[]) {"KB_MENUS" },
 (const __flash char[]) {"KB_RIGHT_CTRL" },  (const __flash char[]) {"KB_INSERT" },  (const __flash char[]) {"KB_HOME" },  (const __flash char[]) {"KB_PAGE_UP" },
 (const __flash char[]) {"KB_DELETE" },  (const __flash char[]) {"KB_END" },  (const __flash char[]) {"KB_PAGE_DOWN" },  (const __flash char[]) {"KB_UP_ARROW" },
 (const __flash char[]) {"KB_LEFT_ARROW" },  (const __flash char[]) {"KB_DOWN_ARROW" },  (const __flash char[]) {"KB_RIGHT_ARROW" },  (const __flash char[]) {"KB_NUMLOCK" },
 (const __flash char[]) {"KB_NUMPAD_DIVIDE" },  (const __flash char[]) {"KB_NUMPAD_MULTIPLY" },  (const __flash char[]) {"KB_NUMPAD_MINUS" },  (const __flash char[]) {"KB_NUMPAD_7" },
 (const __flash char[]) {"KB_NUMPAD_8" },  (const __flash char[]) {"KB_NUMPAD_9" },  (const __flash char[]) {"KB_NUMPAD_PLUS" },  (const __flash char[]) {"KB_NUMPAD_4" },
 (const __flash char[]) {"KB_NUMPAD_5" },  (const __flash char[]) {"KB_NUMPAD_6" },  (const __flash char[]) {"KB_NUMPAD_1" },  (const __flash char[]) {"KB_NUMPAD_2" },
 (const __flash char[]) {"KB_NUMPAD_3" },  (const __flash char[]) {"KB_NUMPAD_0" },  (const __flash char[]) {"KB_NUMPAD_PERIOD" },  (const __flash char[]) {"KB_NUMPAD_ENTER" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },
 (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" },  (const __flash char[]) {"KB_NONE" }, 
};

static const char* read_conversion_table(uint8_t index)
{
    return pgm_read_ptr(conversion + index);
}

static const __flash char ascii_noshift[256] = {
0x00,
0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x60, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x30, 0x00, 0x00, 0x08, 0x09, 0x71,
0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69, 0x6f,
0x70, 0x5b, 0x5d, 0x5c, 0x00, 0x61, 0x73, 0x64,
0x66, 0x67, 0x68, 0x6a, 0x6b, 0x6c, 0x3b, 0x27,
0x00, 0x00, 0x7a, 0x78, 0x63, 0x76, 0x62, 0x6e,
0x6d, 0x2c, 0x2e, 0x2f, 0x00, 0x00, 0x00, 0x00,
0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

void ps2_keyboard_init(void)
{
    ps2_init();
	ps2_write(PS2_RESET);
	ps2_write(PS2_SETDEFAULT);

	ps2_write(PS2_SETLEDS);
	ps2_write(0xff);
    ps2_clear();
}

const char* ps2_keyboard_get_decoded(void)
{
    static bool skip = false;

    uint8_t data = 0;
    while(ps2_read(&data));
    
    if (skip) {
        skip = false;
        return 0;
    }

    if (data == PS2_EXTENDED) {
        return 0;
    }

    if (data == PS2_RELEASED) {
        skip = true;
        return 0;
    }

    uint8_t key = read_normal_table(data);
    //const char* result = read_conversion_table(key);
    uint8_t ascii = pgm_read_byte(ascii_noshift + key);
    if (ascii == 0x00)
        return 0;
    uart_transmit(ascii);
    return 0;
}