#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi.h"
#include "ssd1306.h"
#include "joystick.h"
#include "tictactoe.h"
#include "sd.h"

#define BTN_OFF_BIT      PD3
#define BOOT_ADR         0x7000UL
#define FLAG_SD_REMOVED  0xA5
#define FLAG_BTN_SLEEP   0x5A
#define SD_CHECK_TICKS   100   /* ~2s la 20ms/tick */

static void jump_to_boot(uint8_t flag) {
    cli();
    GPIOR0 = flag;
    ((void (*)(void))BOOT_ADR)();
}

int main(void) {
    DDRD  &= ~(1 << BTN_OFF_BIT);
    PORTD |=  (1 << BTN_OFF_BIT);

    twi_init();
    joystick_init();   /* initializeaza si ADC si pull-up PD2 */
    ssd1306_init();
    ssd1306_clear();

    static ttt_game_t game;
    ttt_init(&game);
    ttt_draw(&game);

    uint8_t    sd_tick  = 0;
    direction_t prev_dir = DIR_NONE;

    while (1) {
        /* buton on/off (PD3) → sleep */
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

        /* buton PD2 — plaseaza X sau restart dupa game over */
        if (!(PIND & (1 << JOY_SW_BIT))) {
            _delay_ms(20);
            if (!(PIND & (1 << JOY_SW_BIT))) {
                while (!(PIND & (1 << JOY_SW_BIT)));  /* asteapta release */
                if (game.game_over) {
                    ttt_init(&game);
                } else {
                    ttt_place(&game);
                }
                ttt_draw(&game);
            }
        }

        /* joystick — muta cursorul o singura data per apasare */
        direction_t d = joystick_read();
        if (d != DIR_NONE && prev_dir == DIR_NONE && !game.game_over)
            ttt_move_cursor(&game, d);
        prev_dir = d;

        _delay_ms(20);
    }
}
