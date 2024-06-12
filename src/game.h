#ifndef GAMEH
#define GAMEH

#include <stdint.h>
#include <stdbool.h>
#include "table.h" 

#define WIDTH 7
#define HEIGHT 6

#define BOTTOM_LEFT UINT64_C(1) 
//game.position & (BOTTOM_LEFT << j (j + (i * (HEIGHT + 1))))
//game.mask & (BOTTOM_LEFT << (j + (i * (HEIGHT + 1))))

typedef struct board{
    uint64_t position;
    uint64_t mask;
    int moves;    
    bool gameOver;
} board_t;

void init_board(board_t *game);
void copy(board_t * copy, board_t *game);
bool canAdd(board_t *game, uint8_t column);
void addChip(board_t *game, uint8_t column);
bool isWinningMove(board_t *game, uint8_t col);
bool alignment(uint64_t pos);
uint64_t key(board_t game);

#endif
