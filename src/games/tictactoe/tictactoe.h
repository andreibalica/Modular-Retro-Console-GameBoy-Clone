#ifndef TICTACTOE_H_
#define TICTACTOE_H_

#include <stdint.h>
#include "joystick.h"   /* direction_t */

typedef struct {
    uint8_t board[9];   /* 0=gol, 1=jucator(X), 2=AI(O) */
    uint8_t cursor;     /* 0-8, pozitia cursorului */
    uint8_t game_over;
    uint8_t winner;     /* 0=in joc, 1=jucator, 2=AI, 3=egal */
} ttt_game_t;

void    ttt_init(ttt_game_t *g);
void    ttt_move_cursor(ttt_game_t *g, direction_t d);
uint8_t ttt_place(ttt_game_t *g);   /* plaseaza X, AI raspunde; returneaza 1 daca mutare valida */
void    ttt_draw(const ttt_game_t *g);

#endif
