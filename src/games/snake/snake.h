#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>
#include "ssd1306.h"
#include "joystick.h"

#define CELL_SIZE    8
#define GRID_COLS    (SSD1306_W / CELL_SIZE)    /* 16 */
#define GRID_ROWS    (SSD1306_H / CELL_SIZE)    /* 8  */
#define MAX_SNAKE    (GRID_COLS * GRID_ROWS)    /* 128 */

typedef struct {
    uint8_t x;
    uint8_t y;
} point_t;

typedef struct {
    point_t     body[MAX_SNAKE];
    uint16_t    length;
    direction_t dir;
    direction_t next_dir;
    point_t     food;
    uint8_t     game_over;
    uint16_t    score;
} snake_game_t;

void snake_init(snake_game_t *g);
void snake_tick(snake_game_t *g);
void snake_set_dir(snake_game_t *g, direction_t d);
void snake_draw(const snake_game_t *g);
void snake_game_over(uint16_t score);

#endif
