#include "hd44780.h"

static void lcd_wait()
{
    DDRB = 0x00;
    PORTD |= (1 << LCD_RW);

lcd_busy:
    PORTD |= (1 << LCD_E);
    asm("nop");
    PORTD &= ~(1 << LCD_E);
    
    if (PINB & 0b10000000)
        goto lcd_busy;

    PORTD &= ~(1 << LCD_RS);
    PORTD &= ~(1 << LCD_RW);
    PORTD &= ~(1 << LCD_E);
    DDRB = 0xff;
}

void lcd_init()
{
    DDRB = 0xff;
    DDRD |= (1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_E);
    PORTD &= ~(1 << LCD_RS);
    PORTD &= ~(1 << LCD_RW);
    PORTD &= ~(1 << LCD_E);

    lcd_write_instruction(0b00111000); // Set 8-bit mode; 2-line display; 5x8 font
    lcd_write_instruction(0b00001100); // Display on; cursor off; blink off
    lcd_write_instruction(0b00000110); // Increment and shift cursor; don't shift display 
    lcd_write_instruction(LCD_DISPLAY_CLEAR);
}

void lcd_home()
{
    lcd_write_instruction(LCD_CURSOR_HOME);
}

void lcd_clear()
{
    lcd_write_instruction(LCD_DISPLAY_CLEAR);
}

void lcd_write_data(uint8_t data)
{
    lcd_wait();
    PORTD |= (1 << LCD_RS);
    PORTB = data;
    PORTD |= (1 << LCD_E);
    asm("nop");
    PORTD &= ~(1 << LCD_E);
    PORTD &= ~(1 << LCD_RS);
}

void lcd_write_instruction(uint8_t instruction)
{
    lcd_wait();
    PORTB = instruction;
    PORTD |= (1 << LCD_E);
    asm("nop");
    PORTD &= ~(1 << LCD_E);
}

void lcd_print(const char* s)
{
    while(*s > 0) lcd_write_data(*s++);
}