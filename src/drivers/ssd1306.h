#ifndef SSD1306_H_
#define SSD1306_H_

#include <avr/io.h>

#define SSD1306_ADDR    0x78    /* 0x3C << 1 */
#define SSD1306_W       128
#define SSD1306_H       64

void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_fill_screen(uint8_t color);            /* 0=negru, 1=alb */
void ssd1306_fill_cell(uint8_t gx, uint8_t gy, uint8_t color); /* celula 8x8 */
void ssd1306_set_cursor(uint8_t col, uint8_t page);
void ssd1306_write_char(char c);
void ssd1306_write_string(const char *s);
void ssd1306_write_col(uint8_t x, const uint8_t data8[8]);

#endif
