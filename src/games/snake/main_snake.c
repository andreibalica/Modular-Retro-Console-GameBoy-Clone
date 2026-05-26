#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi.h"
#include "ssd1306.h"
#include "joystick.h"
#include "snake.h"
#include "sd.h"

#define BTN_OFF_BIT      PD3
#define BOOT_ADR         0x7000UL
#define FLAG_SD_REMOVED  0xA5
#define FLAG_BTN_SLEEP   0x5A
#define SD_CHECK_TICKS   13    /* ~2s la 150ms/tick */

static void jump_to_boot(uint8_t flag) {
    cli();
    GPIOR0 = flag;
    ((void (*)(void))BOOT_ADR)();
}

int main(void) {
    DDRD  &= ~(1 << BTN_OFF_BIT);
    PORTD |=  (1 << BTN_OFF_BIT);

    twi_init();
    joystick_init();
    ssd1306_init();
    ssd1306_clear();

    static snake_game_t game;
    snake_init(&game);
    snake_draw(&game);

    uint8_t sd_tick = 0;

    while (1) {
        /* buton on/off → sleep */
        if (!(PIND & (1 << BTN_OFF_BIT))) {
            _delay_ms(20);
            if (!(PIND & (1 << BTN_OFF_BIT)))
                jump_to_boot(FLAG_BTN_SLEEP);
        }

        /* polling SD la ~2s */
        if (++sd_tick >= SD_CHECK_TICKS) {
            sd_tick = 0;
            if (disk_initialize() & STA_NOINIT)
                jump_to_boot(FLAG_SD_REMOVED);
        }

        direction_t d = joystick_read();
        snake_set_dir(&game, d);

        _delay_ms(150);
        snake_tick(&game);

        if (game.game_over) {
            snake_game_over(game.score);
            snake_init(&game);
            snake_draw(&game);
        }
    }
}
