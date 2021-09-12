#ifndef HD44780_H
#define HD44780_H

#include <avr/io.h>

#define LCD_RS 5
#define LCD_RW 6
#define LCD_E 7

#define LCD_DISPLAY_CLEAR 0b00000001
#define LCD_CURSOR_HOME   0b00000010

void lcd_init();
void lcd_write_data(uint8_t data);
void lcd_write_instruction(uint8_t instruction);

void lcd_home();
void lcd_clear();

void lcd_print(const char* s);

#endif