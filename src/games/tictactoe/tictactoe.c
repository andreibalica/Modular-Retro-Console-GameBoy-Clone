#include <util/delay.h>
#include "tictactoe.h"
#include "ssd1306.h"

/* ---- RNG ----------------------------------------------------------------- */
static uint16_t rng_state = 0xBEEF;
static uint16_t rng16(void) {
    rng_state ^= rng_state << 7;
    rng_state ^= rng_state >> 9;
    rng_state ^= rng_state << 8;
    return rng_state;
}

/* ---- Win detection ------------------------------------------------------- */
static const uint8_t WIN_COMBOS[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},   /* randuri */
    {0,3,6}, {1,4,7}, {2,5,8},   /* coloane */
    {0,4,8}, {2,4,6}             /* diagonale */
};

static uint8_t check_winner(const uint8_t board[9]) {
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t a = WIN_COMBOS[i][0];
        uint8_t b = WIN_COMBOS[i][1];
        uint8_t c = WIN_COMBOS[i][2];
        if (board[a] && board[a] == board[b] && board[b] == board[c])
            return board[a];
    }
    for (uint8_t i = 0; i < 9; i++)
        if (!board[i]) return 0;
    return 3; /* egal - toate casutele ocupate */
}

/* ---- Randare ------------------------------------------------------------- */
/*
 * Layout ecran 128x64:
 *   Page 0:  "  TIC TAC TOE  "   header
 *   Page 1:  (gol)
 *   Page 2:  rand 0 al grilei
 *   Page 3:  "---+---+---"        separator
 *   Page 4:  rand 1 al grilei
 *   Page 5:  "---+---+---"
 *   Page 6:  rand 2 al grilei
 *   Page 7:  status ("BTN=PLAY" / "YOU WIN!" etc.)
 *
 * Fiecare celula = 3 chars = 18px. Separator "|" = 6px.
 * Latime rand: 3*18 + 2*6 = 66px. Marginea stanga: (128-66)/2 = 31px.
 * Pozitii coloane (pixeli): col0=31, col1=55, col2=79.
 */
#define GRID_LEFT  31
#define COL1_PX    55
#define COL2_PX    79

static const uint8_t COL_X[3] = {GRID_LEFT, COL1_PX, COL2_PX};

static void draw_cell(const ttt_game_t *g, uint8_t idx) {
    uint8_t row = idx / 3, col = idx % 3;
    uint8_t is_cur = (g->cursor == idx);
    ssd1306_set_cursor(COL_X[col], 2 + row * 2);
    ssd1306_write_char(is_cur ? '[' : ' ');
    uint8_t v = g->board[idx];
    ssd1306_write_char(v == 1 ? 'X' : v == 2 ? 'O' : ' ');
    ssd1306_write_char(is_cur ? ']' : ' ');
}

/* ---- API publica --------------------------------------------------------- */

void ttt_init(ttt_game_t *g) {
    for (uint8_t i = 0; i < 9; i++) g->board[i] = 0;
    g->cursor    = 4; /* centru */
    g->game_over = 0;
    g->winner    = 0;
}

void ttt_move_cursor(ttt_game_t *g, direction_t d) {
    if (g->game_over) return;
    uint8_t old  = g->cursor;
    uint8_t row  = old / 3;
    uint8_t col  = old % 3;
    switch (d) {
        case DIR_UP:    row = (row + 2) % 3; break;
        case DIR_DOWN:  row = (row + 1) % 3; break;
        case DIR_LEFT:  col = (col + 2) % 3; break;
        case DIR_RIGHT: col = (col + 1) % 3; break;
        default: return;
    }
    g->cursor = row * 3 + col;
    draw_cell(g, old);       /* sterge cursorul vechi */
    draw_cell(g, g->cursor); /* deseneaza cursorul nou */
}

uint8_t ttt_place(ttt_game_t *g) {
    if (g->game_over || g->board[g->cursor]) return 0;

    g->board[g->cursor] = 1; /* jucator = X */
    g->winner = check_winner(g->board);
    if (g->winner) { g->game_over = 1; return 1; }

    /* AI: alege o casuta libera random */
    uint8_t empty[9], cnt = 0;
    for (uint8_t i = 0; i < 9; i++)
        if (!g->board[i]) empty[cnt++] = i;
    if (cnt) {
        g->board[empty[rng16() % cnt]] = 2; /* AI = O */
        g->winner = check_winner(g->board);
        if (g->winner) g->game_over = 1;
    }
    return 1;
}

void ttt_draw(const ttt_game_t *g) {
    ssd1306_clear();

    /* Header */
    ssd1306_set_cursor(GRID_LEFT, 0);
    ssd1306_write_string("TIC TAC TOE");

    /* Grila: 3 randuri + separatoare */
    for (uint8_t row = 0; row < 3; row++) {
        uint8_t page = 2 + row * 2;
        /* Celulele randului */
        ssd1306_set_cursor(GRID_LEFT, page);
        for (uint8_t col = 0; col < 3; col++) {
            uint8_t idx    = row * 3 + col;
            uint8_t is_cur = (g->cursor == idx);
            ssd1306_write_char(is_cur ? '[' : ' ');
            uint8_t v = g->board[idx];
            ssd1306_write_char(v == 1 ? 'X' : v == 2 ? 'O' : ' ');
            ssd1306_write_char(is_cur ? ']' : ' ');
            if (col < 2) ssd1306_write_char('|');
        }
        /* Separator dupa randurile 0 si 1 */
        if (row < 2) {
            ssd1306_set_cursor(GRID_LEFT, page + 1);
            ssd1306_write_string("---+---+---");
        }
    }

    /* Status */
    ssd1306_set_cursor(40, 7);
    if (g->game_over) {
        if (g->winner == 1)      ssd1306_write_string("YOU WIN!");
        else if (g->winner == 2) ssd1306_write_string("AI WINS!");
        else                     ssd1306_write_string(" DRAW!! ");
    } else {
        ssd1306_write_string("BTN=PLAY");
    }
}
