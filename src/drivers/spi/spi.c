#include "spi.h"

//try every prescaler so that the frequency will be
//between SPI_MIN_FREQUENCY and SPI_FREQUENCY
#if SPI_MIN_FREQUENCY <= F_CPU/2 && F_CPU/2 <= SPI_FREQUENCY
    #define SPI_PRESCALER 2
    #define SPR0_VALUE 0
    #define SPR1_VALUE 0
    #define SPI2X_VALUE 1
#elif SPI_MIN_FREQUENCY <= F_CPU/4 && F_CPU/4 <= SPI_FREQUENCY
    #define SPI_PRESCALER 4
    #define SPR0_VALUE 0
    #define SPR1_VALUE 0
    #define SPI2X_VALUE 0
#elif SPI_MIN_FREQUENCY <= F_CPU/8 && F_CPU/8 <= SPI_FREQUENCY
    #define SPI_PRESCALER 8
    #define SPR0_VALUE 1
    #define SPR1_VALUE 0
    #define SPI2X_VALUE 1
#elif SPI_MIN_FREQUENCY <= F_CPU/16 && F_CPU/16 <= SPI_FREQUENCY
    #define SPI_PRESCALER 16
    #define SPR0_VALUE 1
    #define SPR1_VALUE 0
    #define SPI2X_VALUE 0
#elif SPI_MIN_FREQUENCY <= F_CPU/32 && F_CPU/32 <= SPI_FREQUENCY
    #define SPI_PRESCALER 32
    #define SPR0_VALUE 0
    #define SPR1_VALUE 1
    #define SPI2X_VALUE 1
#elif SPI_MIN_FREQUENCY <= F_CPU/64 && F_CPU/64 <= SPI_FREQUENCY
    #define SPI_PRESCALER 64
    #define SPR0_VALUE 1
    #define SPR1_VALUE 1
    #define SPI2X_VALUE 1
#elif SPI_MIN_FREQUENCY <= F_CPU/128 && F_CPU/128 <= SPI_FREQUENCY
    #define SPI_PRESCALER 128
    #define SPR0_VALUE 1
    #define SPR1_VALUE 1
    #define SPI2X_VALUE 0
#else
    #error "No valid SPI_PRESCALER found!"
#endif

#if SPI_MODE == 0
    #define CPOL_VALUE 0
    #define CPHA_VALUE 0
#elif SPI_MODE == 1
    #define CPOL_VALUE 0
    #define CPHA_VALUE 1
#elif SPI_MODE == 2
    #define CPOL_VALUE 1
    #define CPHA_VALUE 0
#elif SPI_MODE == 3
    #define CPOL_VALUE 1
    #define CPHA_VALUE 1
#else
    #error "No valid SPI_MODE defined!"
#endif

#if SPI_DORD == 0
    #define DORD_VALUE 0
#elif SPI_DORD == 1
    #define DORD_VALUE 1
#else
    #error "No valid SPI_DORD defined!"
#endif

void spi_init(void)
{
    DDRB |= (1 << 3) | (1 << 5);

    SPSR |= (SPI2X_VALUE << SPI2X);
    SPCR |= (1 << SPE) | (DORD_VALUE << DORD) | (1 << MSTR)
    | (CPOL_VALUE << CPOL) | (CPHA_VALUE << CPHA)
    | (SPR1_VALUE << SPR1) | (SPR0_VALUE << SPR0);
}

uint8_t spi_writeRead(uint8_t data)
{
    SPDR = data;
    while(~SPSR & (1 << SPIF));
    return SPDR;
}