#include "game.h"

static uint64_t topmask(uint8_t column){
    return (UINT64_C(1) << (HEIGHT - 1)) << column * (HEIGHT + 1);
}

static uint64_t bottommask(uint8_t column){
    return UINT64_C(1) << column * (HEIGHT + 1);
}

void init_board(board_t *game){
    game->position = 0;
    game->mask = 0;
    game->moves = 0;
}

void copy(board_t *copy, board_t *game){
   copy->position = game->position;
   copy->mask = game->mask;
   copy->moves = game->moves;
}

bool canAdd(board_t *game, uint8_t column){
    if(column >= WIDTH) return false;
    return (game->mask & topmask(column)) == 0;
}

void addChip(board_t *game, uint8_t column){
    game->position ^= game->mask;
    game->mask |= game->mask + bottommask(column);
    game->moves++;
}

bool gameOver(board_t *game){
    uint64_t pos = game->position ^ game->mask;

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
