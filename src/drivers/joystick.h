#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <avr/io.h>
#include <stdint.h>

typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
} direction_t;

#define JOY_CH_X    0       /* ADC0 = PC0 */
#define JOY_CH_Y    1       /* ADC1 = PC1 */
#define JOY_SW_PIN  PIND
#define JOY_SW_BIT  PD2

#define JOY_LOW     300
#define JOY_HIGH    700

void joystick_init(void);
direction_t joystick_read(void);

#endif
