#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"

#define SS PB2

void SPI_init() {
    DDRB |= (1 << SPI_MOSI) | (1 << SPI_SCK);
    DDRB |= (1 << SS);
    PORTB |= (1 << SS);
    SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

uint8_t SPI_exchange(uint8_t data) {
    SPDR = data;
    while (bit_is_clear(SPSR, SPIF));
    return SPDR;
}
