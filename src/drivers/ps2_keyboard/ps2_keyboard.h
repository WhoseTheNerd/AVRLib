#pragma once

#include <stdbool.h>
#include <inttypes.h>

typedef enum {
    KB_NONE,
    KB_ESC,
    KB_F1, KB_F2, KB_F3, KB_F4, KB_F5, KB_F6, KB_F7, KB_F8, KB_F9, KB_F10, KB_F11, KB_F12,
    KB_PRTSCR,
    KB_SCROLL_LOCK,
    KB_PAUSE_BREAK,
    KB_GRAVE_ACCENT,
    KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7, KB_8, KB_9, KB_0,
    KB_MINUS, KB_EQUALS,
    KB_BACKSPACE, KB_TAB,
    KB_Q, KB_W, KB_E, KB_R, KB_T, KB_Y, KB_U, KB_I, KB_O, KB_P, KB_LEFT_BRACKET, KB_RIGHT_BRACKET,
    KB_BLACKLASH, KB_CAPSLOCK,
    KB_A, KB_S, KB_D, KB_F, KB_G, KB_H, KB_J, KB_K, KB_L, KB_SEMICOLON, KB_APOSTROPHE,
    KB_ENTER, KB_LEFT_SHIFT, KB_Z, KB_X, KB_C, KB_V, KB_B, KB_N, KB_M, KB_COMMA, KB_PERIOD,
    KB_FORWARD_SLASH, KB_RIGHT_SHIFT, KB_LEFT_CTRL, KB_LEFT_HOST, KB_LEFT_ALT, KB_SPACEBAR, KB_RIGHT_ALT,
    KB_RIGHT_HOST, KB_MENUS, KB_RIGHT_CTRL, KB_INSERT, KB_HOME, KB_PAGE_UP, KB_DELETE, KB_END, KB_PAGE_DOWN,
    KB_UP_ARROW, KB_LEFT_ARROW, KB_DOWN_ARROW, KB_RIGHT_ARROW, KB_NUMLOCK, KB_NUMPAD_DIVIDE, KB_NUMPAD_MULTIPLY,
    KB_NUMPAD_MINUS, KB_NUMPAD_7, KB_NUMPAD_8, KB_NUMPAD_9, KB_NUMPAD_PLUS, KB_NUMPAD_4, KB_NUMPAD_5, KB_NUMPAD_6,
    KB_NUMPAD_1, KB_NUMPAD_2, KB_NUMPAD_3, KB_NUMPAD_0, KB_NUMPAD_PERIOD, KB_NUMPAD_ENTER
} kb_keys_t;

void ps2_keyboard_init(void);
const char* ps2_keyboard_get_decoded(void);