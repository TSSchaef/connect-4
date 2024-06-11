#include "game.h"

static uint64_t topmask(uint8_t column){
    return (UINT64_C(1) << (HEIGHT - 1)) << column * (HEIGHT + 1);
}

static uint64_t bottommask(uint8_t column){
    return UINT64_C(1) << column * (HEIGHT + 1);
}

static uint64_t columnmask(uint8_t column){
    return ((UINT64_C(1) << HEIGHT) - 1) << column * (HEIGHT + 1);
}

void init_board(board_t *game){
    game->position = 0;
    game->mask = 0;
    game->moves = 0;
    game->gameOver = false;
}

void copy(board_t *copy, board_t *game){
   copy->position = game->position;
   copy->mask = game->mask;
   copy->moves = game->moves;
   copy->gameOver = game->gameOver;
}

bool canAdd(board_t *game, uint8_t column){
    if(column >= WIDTH) return false;
    return !(game->mask & topmask(column)) && !game->gameOver;
}

void addChip(board_t *game, uint8_t column){
    if(isWinningMove(game, column)) game->gameOver = true;

    game->position ^= game->mask;
    game->mask |= game->mask + bottommask(column);
    game->moves++;
}

bool isWinningMove(board_t *game, uint8_t col){
    uint64_t pos = game->position;
    pos |= (game->mask + bottommask(col)) & columnmask(col);
    return alignment(pos);
}

bool alignment(uint64_t pos){
    //horizontal
    uint64_t m = pos & (pos >> (HEIGHT + 1));
    if(m & (m >> (2*(HEIGHT + 1)))) return true;

    //diagonal upwards
    m = pos & (pos >> HEIGHT);
    if(m & (m >> (2 * HEIGHT))) return true;

    //diagonal downwards
    m = pos & (pos >> (HEIGHT + 2));
    if(m & (m >> (2 * (HEIGHT + 2)))) return true;

    //vertical
    m = pos & (pos >> 1);
    if(m & (m >> 2)) return true;

    return false;
}
