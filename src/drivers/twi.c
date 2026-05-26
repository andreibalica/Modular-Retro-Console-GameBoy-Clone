#include "twi.h"

void twi_init(void) {
    TWCR = 0;
    TWSR = 0;                       /* prescaler = 1 */
    TWBR = (uint8_t)TWBR_VAL;      /* viteza SCL = 100kHz */
}

void twi_start(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
    while (!(TWCR & (1 << TWINT)));
}

void twi_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void twi_read_ack(uint8_t *data) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    *data = TWDR;
}

void twi_read_nack(uint8_t *data) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    *data = TWDR;
}

void twi_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}
