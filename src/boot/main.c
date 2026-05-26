#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "twi.h"
#include "pff.h"

void flash_erase(DWORD addr);
void flash_write(DWORD addr, const BYTE *buf);

#define OLED     0x78
#define LED_BIT  PB1
#define BTN_BIT  PD3

/* ---- OLED (adaptat din ssd1306.c) ---- */

static void ssd1306_send_cmds(const uint8_t *p, uint8_t n) {
	twi_start(); twi_write(OLED); twi_write(0x00);
	while (n--) twi_write(pgm_read_byte(p++));
	twi_stop();
}

static const uint8_t SEQ_INIT[] PROGMEM = {
	0xAE,0xD5,0x80,0xA8,0x3F,0xD3,0x00,0x40,
	0x8D,0x14,0x20,0x00,0xA1,0xC8,0xDA,0x12,
	0x81,0xCF,0xD9,0xF1,0xDB,0x40,0xA4,0xA6,0xAF
};
static const uint8_t SEQ_FULL[]  PROGMEM = {0x21,0,127,0x22,0,7};
static const uint8_t SEQ_PAGE3[] PROGMEM = {0x21,31,127,0x22,3,3};

static void ssd1306_init(void) { ssd1306_send_cmds(SEQ_INIT, 25); }
static void ssd1306_off(void)  { twi_start(); twi_write(OLED); twi_write(0x00); twi_write(0xAE); twi_stop(); }
static void ssd1306_on(void)   { twi_start(); twi_write(OLED); twi_write(0x00); twi_write(0xAF); twi_stop(); }

static void ssd1306_clear(void) {
	ssd1306_send_cmds(SEQ_FULL, 6);
	twi_start(); twi_write(OLED); twi_write(0x40);
	for (uint16_t i = 0; i < 1024; i++) twi_write(0x00);
	twi_stop();
}

/* "INSERT GAME" pre-randat, font 5x8, centrat pe pagina 3 */
static const uint8_t TXT_INSERT_GAME[65] PROGMEM = {
	0x00,0x41,0x7F,0x41,0x00,0x00,  /* I */
	0x7F,0x04,0x08,0x10,0x7F,0x00,  /* N */
	0x46,0x49,0x49,0x49,0x31,0x00,  /* S */
	0x7F,0x49,0x49,0x49,0x41,0x00,  /* E */
	0x7F,0x09,0x19,0x29,0x46,0x00,  /* R */
	0x01,0x01,0x7F,0x01,0x01,0x00,  /* T */
	0x00,0x00,0x00,0x00,0x00,0x00,  /* ' ' */
	0x3E,0x41,0x49,0x49,0x7A,0x00,  /* G */
	0x7E,0x11,0x11,0x11,0x7E,0x00,  /* A */
	0x7F,0x02,0x0C,0x02,0x7F,0x00,  /* M */
	0x7F,0x49,0x49,0x49,0x41         /* E */
};

static void show_insert_screen(void) {
	ssd1306_clear();
	ssd1306_send_cmds(SEQ_PAGE3, 6);
	twi_start(); twi_write(OLED); twi_write(0x40);
	for (uint8_t i = 0; i < 65; i++) twi_write(pgm_read_byte(&TXT_INSERT_GAME[i]));
	twi_stop();
}

/* ---- Buton PD3 — polling simplu, fara ISR ---- */
static void wait_btn_press(void) {
	while ( (PIND & (1 << BTN_BIT)));
	while (!(PIND & (1 << BTN_BIT)));
}

static void jump_to_app(void) {
	MCUCR = (1 << IVCE);
	MCUCR = 0;
	((void (*)(void))0x0000)();
}

static uint8_t load_firmware(void) {
	static BYTE buf[128];
	WORD br; DWORD addr = 0;
	do {
		if (pf_read(buf, 128, &br) != FR_OK) return 0;
		if (br == 0) break;
		while (br < 128) buf[br++] = 0xFF;
		flash_erase(addr);
		flash_write(addr, buf);
		addr += 128;
		PORTB ^= (1 << LED_BIT);
	} while (addr < 0x7000UL);
	return 1;
}

#define FLAG_SD_REMOVED  0xA5
#define FLAG_BTN_SLEEP   0x5A

int main(void) {
	MCUSR = 0;
	wdt_disable();

	uint8_t flag = GPIOR0;
	GPIOR0 = 0;

	DDRB  |=  (1 << LED_BIT);
	DDRD  &= ~(1 << BTN_BIT);
	PORTD |=  (1 << BTN_BIT);
	PORTB &= ~(1 << LED_BIT);
	PORTB |=  (1 << PB4);

	if (flag == 0)
		wait_btn_press();   /* cold boot: asteapta apasare inainte de orice */

	twi_init();
	ssd1306_init();

	if (flag == FLAG_BTN_SLEEP) {
		ssd1306_off();
		PORTB &= ~(1 << LED_BIT);
		while (!(PIND & (1 << BTN_BIT)));
		_delay_ms(50);
		wait_btn_press();
	}

awake:
	PORTB |= (1 << LED_BIT);
	ssd1306_on();
	show_insert_screen();

	static FATFS fs;

	while (1) {
		if (pf_mount(&fs) == FR_OK && pf_open("FIRMWARE.BIN") == FR_OK) {
			ssd1306_clear();
			if (load_firmware()) {
					PORTB |= (1 << LED_BIT);
					jump_to_app();
			} else {
					show_insert_screen();
			}
		}

		if (!(PIND & (1 << BTN_BIT))) {
			while (!(PIND & (1 << BTN_BIT)));
			ssd1306_off();
			PORTB &= ~(1 << LED_BIT);
			wait_btn_press();
			goto awake;
		}
		_delay_ms(20);
	}
}
