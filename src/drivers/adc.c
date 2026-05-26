#include <avr/io.h>
#include "adc.h"

void adc_init(void) {
    ADMUX  |= (1 << REFS0);                            /* referinta AVCC */
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); /* prescaler 128 */
    ADCSRA |= (1 << ADEN);                             /* enable ADC */
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x07);         /* selecteaza canalul */
    ADCSRA |= (1 << ADSC);                             /* start conversie */
    while (ADCSRA & (1 << ADSC));                      /* asteapta */
    return ADC;
}
