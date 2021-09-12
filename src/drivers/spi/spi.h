#pragma once

#include <avr/io.h>
	
//default to 1MHz
#ifndef SPI_FREQUENCY
    #define SPI_FREQUENCY 1000000
#endif

//set lower limit if needed
#ifndef SPI_MIN_FREQUENCY
    #define SPI_MIN_FREQUENCY 0
#endif

//datasheet table 19-2
//(CPOL << 1) || (CPHA << 0)
#ifndef SPI_MODE
    #define SPI_MODE 0
#endif

//0: MSB first, 1: LSB first
#ifndef SPI_DORD
    #define SPI_DORD 0
#endif

void spi_init(void);

uint8_t spi_writeRead(uint8_t data);