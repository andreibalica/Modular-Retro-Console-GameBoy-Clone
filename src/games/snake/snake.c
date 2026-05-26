#include <util/delay.h>
#include "ssd1306.h"
#include "snake.h"

static uint16_t rng_state = 0xACE1;

static uint16_t xorshift16(void) {
    rng_state ^= rng_state << 7;
    rng_state ^= rng_state >> 9;
    rng_state ^= rng_state << 8;
    return rng_state;
}

static uint8_t random_range(uint8_t max) {
    return xorshift16() % max;
}

static void place_food(snake_game_t *g) {
    uint8_t x, y, collision;
    do {
        x = random_range(GRID_COLS);
        y = random_range(GRID_ROWS);
        collision = 0;
        for (uint16_t i = 0; i < g->length; i++) {
            if (g->body[i].x == x && g->body[i].y == y) {
                collision = 1;
                break;
            }
        }
    } while (collision);
    g->food.x = x;
    g->food.y = y;
}

void snake_init(snake_game_t *g) {
    g->length    = 3;
    g->dir       = DIR_RIGHT;
    g->next_dir  = DIR_RIGHT;
    g->game_over = 0;
    g->score     = 0;

    uint8_t start_x = GRID_COLS / 4;
    uint8_t start_y = GRID_ROWS / 2;
    for (uint16_t i = 0; i < g->length; i++) {
        g->body[i].x = start_x - i;
        g->body[i].y = start_y;
    }

    place_food(g);
}

void snake_set_dir(snake_game_t *g, direction_t d) {
    if (d == DIR_NONE) return;
    if (d == DIR_UP    && g->dir == DIR_DOWN)  return;
    if (d == DIR_DOWN  && g->dir == DIR_UP)    return;
    if (d == DIR_LEFT  && g->dir == DIR_RIGHT) return;
    if (d == DIR_RIGHT && g->dir == DIR_LEFT)  return;
    g->next_dir = d;
}

void snake_tick(snake_game_t *g) {
    if (g->game_over) return;

    g->dir = g->next_dir;

    point_t new_head = g->body[0];
    switch (g->dir) {
        case DIR_UP:    if (new_head.y == 0) new_head.y = GRID_ROWS - 1; else new_head.y--; break;
        case DIR_DOWN:  if (new_head.y == GRID_ROWS - 1) new_head.y = 0; else new_head.y++; break;
        case DIR_LEFT:  if (new_head.x == 0) new_head.x = GRID_COLS - 1; else new_head.x--; break;
        case DIR_RIGHT: if (new_head.x == GRID_COLS - 1) new_head.x = 0; else new_head.x++; break;
        default: break;
    }

    for (uint16_t i = 0; i < g->length; i++) {
        if (g->body[i].x == new_head.x && g->body[i].y == new_head.y) {
            g->game_over = 1;
            return;
        }
    }

    uint8_t ate = (new_head.x == g->food.x && new_head.y == g->food.y);

    if (!ate) {
        point_t tail = g->body[g->length - 1];
        ssd1306_fill_cell(tail.x, tail.y, 0);
        for (uint16_t i = g->length - 1; i > 0; i--)
            g->body[i] = g->body[i - 1];
    } else {
        if (g->length < MAX_SNAKE) {
            for (uint16_t i = g->length; i > 0; i--)
                g->body[i] = g->body[i - 1];
            g->length++;
        }
        g->score += 10;
        place_food(g);
        ssd1306_fill_cell(g->food.x, g->food.y, 1);
    }

    g->body[0] = new_head;
    ssd1306_fill_cell(new_head.x, new_head.y, 1);
}

void snake_draw(const snake_game_t *g) {
    ssd1306_clear();
    ssd1306_fill_cell(g->food.x, g->food.y, 1);
    for (uint16_t i = 0; i < g->length; i++)
        ssd1306_fill_cell(g->body[i].x, g->body[i].y, 1);
}

void snake_game_over(uint16_t score) {
    (void)score;
    for (uint8_t i = 0; i < 4; i++) {
        ssd1306_fill_screen(1);
        _delay_ms(150);
        ssd1306_fill_screen(0);
        _delay_ms(150);
    }
}
