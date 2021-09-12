#ifndef TB6612_H
#define TB6612_H

#include <avr/io.h>

void tb6612_init(void);

void tb6612_set1(int16_t speed);
void tb6612_set2(int16_t speed);
void tb6612_stop(void);

#endif