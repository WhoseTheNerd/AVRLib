#include "TB6612.h"

/*
 * PD4 = DIR2
 * PD5 = PWM2
 * PD6 = PWM1
 * PD7 = DIR1
*/

static void enable_dir1(void)
{
    PORTD |= (1 << 7);
}

static void disable_dir1(void)
{
    PORTD &= ~(1 << 7);
}

static void enable_dir2(void)
{
    PORTD |= (1 << 4);
}

static void disable_dir2(void)
{
    PORTD &= ~(1 << 4);
}

static void speed_1(uint8_t speed)
{
    OCR0A = speed;
}

static void speed_2(uint8_t speed)
{
    OCR0B = speed;
}

static void enable_pwm(void)
{
    TCCR0A |= (1 << COM0A1) | (1 << COM1A1) | (1 << COM0B1) | (1 << COM1B1) | (1 << WGM00);
    TCCR0B |= (1 << CS00);
}

static void disable_pwm(void)
{
    TCCR0A = 0;
    TCCR0B = 0;
}

static void brake(void)
{
    PORTD &= ~(1 << 5);
    PORTD &= ~(1 << 6);
}

void tb6612_init(void)
{
    DDRD |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7); // PD5, PD6, PD7

    enable_pwm();
    speed_1(0);
    speed_2(0);
    disable_dir1();
    disable_dir2();
}

void tb6612_set1(int16_t speed)
{
    enable_pwm();
    
    speed = speed > 255 ? 255 : speed;
    speed = speed < -255 ? -255 : speed;

    if (speed >= 0)  {
        disable_dir1();
        speed_1(speed);
    } 
    else {
        enable_dir1();
        speed_1(-speed);
    }
}

void tb6612_set2(int16_t speed)
{
    enable_pwm();

    speed = speed > 255 ? 255 : speed;
    speed = speed < -255 ? -255 : speed;

    if (speed >= 0)  {
        enable_dir2();
        speed_2(speed);
    } 
    else {
        disable_dir2();
        speed_2(-speed);
    }
}

void tb6612_stop(void)
{
    speed_1(0);
    speed_2(0);
    disable_pwm();
    brake();
}