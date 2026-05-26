#ifndef TWI_H_
#define TWI_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/twi.h>

#define F_SCL       100000UL
#define PRESCALER   1
#define TWBR_VAL    ((((F_CPU / F_SCL) / PRESCALER) - 16) / (2 * PRESCALER))

void twi_init(void);
void twi_start(void);
void twi_write(uint8_t data);
void twi_read_ack(uint8_t *data);
void twi_read_nack(uint8_t *data);
void twi_stop(void);

#endif
