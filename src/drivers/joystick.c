#include "joystick.h"
#include "adc.h"

void joystick_init(void) {
    adc_init();
    DDRD  &= ~(1 << JOY_SW_BIT);   /* SW ca intrare */
    PORTD |=  (1 << JOY_SW_BIT);   /* pull-up intern */
}

direction_t joystick_read(void) {
    uint16_t x = adc_read(JOY_CH_X);
    uint16_t y = adc_read(JOY_CH_Y);

    /* Joystick rotit fizic 90° CW: inainte=dreapta, dreapta=jos, jos=stanga, stanga=sus */
    if (x < JOY_LOW)  return DIR_UP;
    if (x > JOY_HIGH) return DIR_DOWN;
    if (y < JOY_LOW)  return DIR_RIGHT;
    if (y > JOY_HIGH) return DIR_LEFT;

    return DIR_NONE;
}
