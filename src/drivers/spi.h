#ifndef SPI_H
#define SPI_H

#include <stdint.h>

/* SPI config — ATmega328P (MOSI=PB3, MISO=PB4, SCK=PB5, SS=PB2) */
#define SPI_PORT  PORTB
#define SPI_DDR   DDRB
#define SPI_MISO  PB4
#define SPI_MOSI  PB3
#define SPI_SCK   PB5
#define SPI_SS    PB2

void    SPI_init(void);
uint8_t SPI_exchange(uint8_t data);

#endif // SPI_H
